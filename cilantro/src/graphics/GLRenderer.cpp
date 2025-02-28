#include "graphics/GLRenderer.h"
#include "graphics/GLShader.h"
#include "graphics/GLShaderProgram.h"
#include "graphics/GLFramebuffer.h"
#if (CILANTRO_GL_VERSION > 140)
#include "graphics/GLMultisampleFramebuffer.h"
#endif
#include "graphics/QuadRenderStage.h"

#include "system/Game.h"
#include "math/Mathf.h"
#include "math/Vector4f.h"
#include "math/Matrix3f.h"
#include "math/Matrix4f.h"
#include "scene/GameScene.h"
#include "scene/MeshObject.h"
#include "scene/Camera.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "scene/SpotLight.h"
#include "glad/glad.h"
#include <cmath>
#include <cstring>

CGLRenderer::CGLRenderer (CGameScene* gameScene, unsigned int width, unsigned int height, bool shadowMappingEnabled, bool deferredRenderingEnabled) 
    : CRenderer (gameScene, width, height, shadowMappingEnabled, deferredRenderingEnabled)
{
    m_quadGeometryBuffer = new SGlGeometryBuffers ();
    m_uniformBuffers = new SGlUniformBuffers ();
    m_uniformMatrixBuffer = new SGlUniformMatrixBuffer ();
    m_uniformPointLightBuffer = new SGlUniformPointLightBuffer ();
    m_uniformDirectionalLightBuffer = new SGlUniformDirectionalLightBuffer ();
    m_uniformSpotLightBuffer = new SGlUniformSpotLightBuffer ();
}

CGLRenderer::~CGLRenderer ()
{
    for (auto&& objectBuffer : m_sceneGeometryBuffers)
    {
        delete objectBuffer.second;
    }

    delete m_quadGeometryBuffer;
    delete m_uniformBuffers;
    delete m_uniformMatrixBuffer;
    delete m_uniformPointLightBuffer;
    delete m_uniformDirectionalLightBuffer;
    delete m_uniformSpotLightBuffer;
}

void CGLRenderer::Initialize ()
{    
    CRenderer::Initialize ();

    InitializeShaderLibrary ();
    InitializeQuadGeometryBuffer ();
    InitializeObjectBuffers ();
    InitializeMatrixUniformBuffers ();
    InitializeLightUniformBuffers ();

    // set callback for new MeshObjects
    CGame::GetMessageBus ().Subscribe<MeshObjectUpdateMessage> (
        [&](const std::shared_ptr<MeshObjectUpdateMessage>& message) 
        { 
            Update (m_gameScene->GetGameObjectManager ().GetByHandle<MeshObject> (message->GetHandle ()));
        }
    );

    // set callback for new or modified materials
    CGame::GetMessageBus ().Subscribe<MaterialTextureUpdateMessage> (
        [&](const std::shared_ptr<MaterialTextureUpdateMessage>& message) 
        { 
            Update (m_gameScene->GetMaterialManager ().GetByHandle<Material> (message->GetHandle ()), message->GetTextureUnit ());
        }
    );
    CGame::GetMessageBus ().Subscribe<MaterialUpdateMessage> (
        [&](const std::shared_ptr<MaterialUpdateMessage>& message) 
        { 
            Update (m_gameScene->GetMaterialManager ().GetByHandle<Material> (message->GetHandle ()));
        }
    );
    
    // set callback for new or modified lights
    CGame::GetMessageBus ().Subscribe<LightUpdateMessage> (
        [&](const std::shared_ptr<LightUpdateMessage>& message) 
        { 
            m_gameScene->GetGameObjectManager ().GetByHandle<GameObject> (message->GetHandle ()).OnUpdate (*this); 
        }
    );

    // set callback for modified scene graph (currently this only requires to reload light buffers)
    CGame::GetMessageBus ().Subscribe<SceneGraphUpdateMessage> (
        [&](const std::shared_ptr<SceneGraphUpdateMessage>& message) 
        { 
            UpdateLightBufferRecursive (message->GetHandle ());
        }
    );

    // set callback for modified transforms (currently this only requires to reload light buffers)
    CGame::GetMessageBus ().Subscribe<TransformUpdateMessage> (
        [&](const std::shared_ptr<TransformUpdateMessage>& message) 
        { 
            UpdateLightBufferRecursive (message->GetHandle ());
        }
    );
    
}

void CGLRenderer::Deinitialize ()
{
    CRenderer::Deinitialize ();

    DeinitializeQuadGeometryBuffer ();
    DeinitializeObjectBuffers ();
    DeinitializeMatrixUniformBuffers ();
    DeinitializeLightUniformBuffers ();
}

IRenderer& CGLRenderer::SetViewport (unsigned int x, unsigned int y, unsigned int sx, unsigned int sy)
{
    glViewport (x, y, sx, sy);

    return *this;
}

void CGLRenderer::RenderFrame ()
{
    CRenderer::RenderFrame ();
}

void CGLRenderer::Draw (MeshObject& meshObject)
{
    GLuint shaderProgramId;

    Material& objM = meshObject.GetMaterial ();

    // get shader program for rendered meshobject (geometry pass)
    CGLShaderProgram& geometryShaderProgram = m_shaderProgramManager.GetByName<CGLShaderProgram> (
        m_isDeferredRendering
        ? objM.GetDeferredGeometryPassShaderProgram ()
        : objM.GetForwardShaderProgram ()
    );
    geometryShaderProgram.Use ();
    shaderProgramId = geometryShaderProgram.GetProgramId ();

    // bind textures for active material and bind a shadow map
    if (m_materialTextureUnits.find(meshObject.GetMaterial ().GetHandle ()) != m_materialTextureUnits.end ())
    {
        SGlMaterialTextureUnits* u = m_materialTextureUnits[meshObject.GetMaterial ().GetHandle ()];

        for (GLuint i = 0; i < u->unitsCount; i++)
        {
            glActiveTexture (GL_TEXTURE0 + i);
            glBindTexture (GL_TEXTURE_2D, u->textureUnits[i]);
        }

        // bind shadow map (if exists) on next available texture slot
        if (m_isShadowMapping && (GetCurrentRenderStage ()->GetLinkedDepthArrayFramebuffer ()) != nullptr)
        {
            if (GetCurrentRenderStage ()->GetLinkedDepthArrayFramebuffer ()->IsDepthArrayEnabled ())
            {
                GetCurrentRenderStage ()->GetLinkedDepthArrayFramebuffer ()->BindFramebufferDepthArrayTextureAsColor (u->unitsCount);
            }
        }

    }
    else
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Missing texture for object" << meshObject.GetHandle ();
    }

    // set material uniforms for active material
    for (auto&& property : meshObject.GetMaterial ().GetPropertiesMap ())
    {
        if (geometryShaderProgram.HasUniform (property.first.c_str ()))
        {
            if (property.second.size () == 1)
            {
                geometryShaderProgram.SetUniformFloat (property.first.c_str (), property.second[0]);
            }
            else if ((property.second.size () == 3))
            {
                geometryShaderProgram.SetUniformFloatv (property.first.c_str (), property.second.data (), 3);
            }
            else
            {
                LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Invalid vector size for material property" << property.first << "in shader" << geometryShaderProgram.GetName () << "for" << meshObject.GetName ();
            }
        }        
        else 
        {
            LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Invalid material uniform" << property.first << "in shader" << geometryShaderProgram.GetName () << "for" << meshObject.GetName ();
        }
    }

    // get world matrix for drawn objects and set uniform value
    geometryShaderProgram.SetUniformMatrix4f ("mModel", meshObject.GetModelTransformMatrix ());

    // calculate normal matrix for drawn objects and set uniform value
    geometryShaderProgram.SetUniformMatrix3f ("mNormal", Mathf::Invert (Mathf::Transpose (Matrix3f (meshObject.GetModelTransformMatrix ()))));

    // get camera position in world space and set uniform value
    if (!m_isDeferredRendering)
    {
        geometryShaderProgram.SetUniformVector3f ("eyePosition", m_gameScene->GetActiveCamera ()->GetPosition ());
    }

    // set bone transformation matrix array uniform
    geometryShaderProgram.SetUniformMatrix4fv ("mBoneTransformations", meshObject.GetBoneTransformationsMatrixArray (), CILANTRO_MAX_BONES);

    // get shader program for rendered meshobject (lighting pass)
    if (m_isDeferredRendering)
    {
        CGLShaderProgram& lightingShaderProgram = m_shaderProgramManager.GetByName<CGLShaderProgram>(objM.GetDeferredLightingPassShaderProgram ());
        lightingShaderProgram.Use ();
        shaderProgramId = lightingShaderProgram.GetProgramId ();

        // get camera position in world space and set uniform value (this needs to be done again for deferred lighting shader program)
        lightingShaderProgram.SetUniformVector3f ("eyePosition", m_gameScene->GetActiveCamera ()->GetPosition ());

    }
    
    // draw mesh
    SGlGeometryBuffers* b = m_sceneGeometryBuffers[meshObject.GetHandle ()];
    geometryShaderProgram.Use ();
    RenderGeometryBuffer (b);
}

void CGLRenderer::DrawQuad ()
{
    RenderGeometryBuffer (m_quadGeometryBuffer);
}

void CGLRenderer::DrawAllGeometryBuffers (IShaderProgram& shader)
{
    shader.Use ();

    for (auto&& geomertyBuffer : m_sceneGeometryBuffers)
    {
        MeshObject m = m_gameScene->GetGameObjectManager ().GetByHandle<MeshObject> (geomertyBuffer.first);

        // load model matrix to currently bound shader
        shader.SetUniformMatrix4f ("mModel", m.GetModelTransformMatrix ());

        // set bone transformation matrix array uniform
        shader.SetUniformMatrix4fv ("mBoneTransformations", m.GetBoneTransformationsMatrixArray (), CILANTRO_MAX_BONES);

        // draw
        RenderGeometryBuffer (geomertyBuffer.second);
    }
}

void CGLRenderer::Update (MeshObject& meshObject)
{
    handle_t objectHandle = meshObject.GetHandle ();

    // check of object's buffers are already initialized
    auto find = m_sceneGeometryBuffers.find (objectHandle);

    if (find == m_sceneGeometryBuffers.end ())
    {
        // it is a new object, so generate buffers 
        SGlGeometryBuffers* b = new SGlGeometryBuffers ();
        m_sceneGeometryBuffers.insert ({ objectHandle, b });

        // generate and bind Vertex Array Object (VAO)
        glGenVertexArrays (1, &b->VAO);
        glBindVertexArray (b->VAO);

        // generate vertex buffer
        glGenBuffers (1, &b->VBO[EGlVboType::VBO_VERTICES]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVboType::VBO_VERTICES]);
        // location = 0 (vertex position)
        glVertexAttribPointer (EGlVboType::VBO_VERTICES, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate normals buffer
        glGenBuffers (1, &b->VBO[EGlVboType::VBO_NORMALS]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVboType::VBO_NORMALS]);
        // location = 1 (vertex normal)
        glVertexAttribPointer (EGlVboType::VBO_NORMALS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate uv buffer
        glGenBuffers (1, &b->VBO[EGlVboType::VBO_UVS]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVboType::VBO_UVS]);
        // location = 2 (vertex uv)
        glVertexAttribPointer (EGlVboType::VBO_UVS, 2, GL_FLOAT, GL_FALSE, 2 * sizeof (float), (GLvoid*)0);

        // generate tangents buffer
        glGenBuffers (1, &b->VBO[EGlVboType::VBO_TANGENTS]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVboType::VBO_TANGENTS]);
        // location = 3 (vertex tangent)
        glVertexAttribPointer (EGlVboType::VBO_TANGENTS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate bitangents buffer
        glGenBuffers (1, &b->VBO[EGlVboType::VBO_BITANGENTS]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVboType::VBO_BITANGENTS]);
        // location = 4 (vertex bitangent)
        glVertexAttribPointer (EGlVboType::VBO_BITANGENTS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate bone index
        glGenBuffers (1, &b->VBO[EGlVboType::VBO_BONES]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVboType::VBO_BONES]);
        // location = 5 (bone indices)
        glVertexAttribIPointer (EGlVboType::VBO_BONES, CILANTRO_MAX_BONE_INFLUENCES, GL_INT, CILANTRO_MAX_BONE_INFLUENCES * sizeof (int), (GLvoid*)0);

        // generate bone weight buffer
        glGenBuffers (1, &b->VBO[EGlVboType::VBO_BONEWEIGHTS]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVboType::VBO_BONEWEIGHTS]);
        // location = 6 (bone weights)
        glVertexAttribPointer (EGlVboType::VBO_BONEWEIGHTS, CILANTRO_MAX_BONE_INFLUENCES, GL_FLOAT, GL_FALSE, CILANTRO_MAX_BONE_INFLUENCES * sizeof (int), (GLvoid*)0);

        // generate index buffer
        glGenBuffers (1, &b->EBO);
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, b->EBO);

        // enable VBO arrays
        glEnableVertexAttribArray (EGlVboType::VBO_VERTICES);
        glEnableVertexAttribArray (EGlVboType::VBO_NORMALS);
        glEnableVertexAttribArray (EGlVboType::VBO_UVS);
        glEnableVertexAttribArray (EGlVboType::VBO_TANGENTS);
        glEnableVertexAttribArray (EGlVboType::VBO_BITANGENTS);
        glEnableVertexAttribArray (EGlVboType::VBO_BONES);
        glEnableVertexAttribArray (EGlVboType::VBO_BONEWEIGHTS);

        // unbind VAO
        glBindVertexArray (0);
    }

    // resize buffers and load data
    SGlGeometryBuffers* b = m_sceneGeometryBuffers[objectHandle];
    b->indexCount = meshObject.GetMesh ().GetIndexCount ();

    // bind Vertex Array Object (VAO)
    glBindVertexArray (b->VAO);

    // load vertex buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVboType::VBO_VERTICES]);
    glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 3, meshObject.GetMesh ().GetVerticesData (), GL_STATIC_DRAW);

    // load normals buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVboType::VBO_NORMALS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 3, meshObject.GetMesh ().GetNormalsData (), GL_STATIC_DRAW);
    
    // load uv buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVboType::VBO_UVS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 2, meshObject.GetMesh ().GetUVData (), GL_STATIC_DRAW);

    // load tangents buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVboType::VBO_TANGENTS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 3, meshObject.GetMesh ().GetTangentData (), GL_STATIC_DRAW);

    // load bitangents buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVboType::VBO_BITANGENTS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 3, meshObject.GetMesh ().GetBitangentData (), GL_STATIC_DRAW);

    // load bone index buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVboType::VBO_BONES]);
    glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (uint32_t) * CILANTRO_MAX_BONE_INFLUENCES, meshObject.GetMesh ().GetBoneIndicesData (), GL_STATIC_DRAW);

    // load bone weight buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVboType::VBO_BONEWEIGHTS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * CILANTRO_MAX_BONE_INFLUENCES, meshObject.GetMesh ().GetBoneWeightsData (), GL_STATIC_DRAW);

    // load index buffer
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, b->EBO);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, meshObject.GetMesh ().GetIndexCount () * sizeof (uint32_t), meshObject.GetMesh ().GetFacesData (), GL_STATIC_DRAW);

    // unbind VAO
    glBindVertexArray (0);

}

void CGLRenderer::Update (Material& material, unsigned int textureUnit)
{
    handle_t materialHandle = material.GetHandle ();
    GLuint texture;
    GLuint format;

    auto GLTextureFormat = [](unsigned int numChannels)
    {
        switch (numChannels)
        {
        case 1:
            return GL_RED;
            break;
        case 3:
            return GL_RGB;
            break;
        case 4:
            return GL_RGBA;
            break;
        default:
            return GL_RGB;
        }
    };

    texture_map_t& textures = material.GetTexturesMap ();

    // check if material already exists
    auto find = m_materialTextureUnits.find (materialHandle);

    if (find == m_materialTextureUnits.end ())
    {
        m_materialTextureUnits.insert ({ materialHandle, new SGlMaterialTextureUnits () });
        
        for (auto&& t : textures)
        {
            Texture* tPtr = t.second.second;
            std::string tName = t.second.first;
            GLuint unit = t.first;
            format = GLTextureFormat (tPtr->GetChannels ());

            glGenTextures (1, &texture);
            glBindTexture (GL_TEXTURE_2D, texture);
            glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D (GL_TEXTURE_2D, 0, format, tPtr->GetWidth (), tPtr->GetHeight (), 0, format, GL_UNSIGNED_BYTE, tPtr->Data ());
            glGenerateMipmap (GL_TEXTURE_2D);
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture (GL_TEXTURE_2D, 0);
            
            m_materialTextureUnits[materialHandle]->textureUnits[unit] = texture;
        }

        m_materialTextureUnits[materialHandle]->unitsCount = (unsigned int) textures.size ();

    }
    else
    {
        auto& t = textures[textureUnit];
        Texture* tPtr = t.second;
        std::string tName = t.first;
        GLuint unit = textureUnit;
        format = GLTextureFormat (tPtr->GetChannels ());

        glBindTexture (GL_TEXTURE_2D, m_materialTextureUnits[materialHandle]->textureUnits[unit]);
        glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D (GL_TEXTURE_2D, 0, format, tPtr->GetWidth (), tPtr->GetHeight (), 0, format, GL_UNSIGNED_BYTE, tPtr->Data ());
        glGenerateMipmap (GL_TEXTURE_2D);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture (GL_TEXTURE_2D, 0);
    }

}

void CGLRenderer::Update (Material& material)
{
    handle_t shaderProgramHandle = m_shaderProgramManager.GetByName<CShaderProgram>(material.GetDeferredLightingPassShaderProgram ()).GetHandle ();
    std::string shaderProgramName = material.GetDeferredLightingPassShaderProgram ();

    if (m_isDeferredRendering)
    {
        // add material's shader program to set of used shader programs handles
        // add lighting deferred pass renderStages for each program
        if (m_lightingShaders.find (shaderProgramHandle) == m_lightingShaders.end ())
        {
            // first rotate the pipeline to the left so that geometry stage is last
            RotateRenderPipelineLeft ();
            if (m_isShadowMapping)
            {
                RotateRenderPipelineLeft ();
            }

            // create and append new lighting stage
            m_lightingShaderStagesCount++;
            m_lightingShaders.insert (shaderProgramHandle);
            CQuadRenderStage& p = AddRenderStage <CQuadRenderStage> ("deferred_lighting_" + shaderProgramName);
            p.SetShaderProgram (shaderProgramName);
            p.SetStencilTestEnabled (true).SetStencilTest (EStencilTestFunction::FUNCTION_EQUAL, static_cast<int> (shaderProgramHandle));
            p.SetClearColorOnFrameEnabled (true);
            p.SetClearDepthOnFrameEnabled (true);
            p.SetClearStencilOnFrameEnabled (false);
            p.SetDepthTestEnabled (false);
            p.SetColorAttachmentsFramebufferLink (m_isShadowMapping ? EPipelineLink::LINK_SECOND : EPipelineLink::LINK_FIRST);
            p.SetDepthStencilFramebufferLink (m_isShadowMapping ? EPipelineLink::LINK_SECOND : EPipelineLink::LINK_FIRST);
            p.SetDepthArrayFramebufferLink (m_isShadowMapping ? EPipelineLink::LINK_FIRST : EPipelineLink::LINK_CURRENT);
            p.SetOutputFramebufferLink (m_isShadowMapping ? EPipelineLink::LINK_THIRD : EPipelineLink::LINK_SECOND);
            p.SetFramebufferEnabled (true);

            p.Initialize ();

            // rotate pipeline to the right, so that ultimately geometry stage is first and newly added stage is second
            RotateRenderPipelineRight ();
            RotateRenderPipelineRight ();
            if (m_isShadowMapping)
            {
                RotateRenderPipelineRight ();
            }
            
            // update flags of other deferred lighting stages (if present)
            if (m_lightingShaderStagesCount > 1)
            {
                handle_t stageHandle = GetRenderPipeline ()[2 + (m_isShadowMapping ? 1 : 0)];

                CQuadRenderStage& stage = m_renderStageManager.GetByHandle<CQuadRenderStage> (stageHandle);
                stage.SetClearColorOnFrameEnabled (false);
                stage.SetFramebufferEnabled (false);
            }

            // update pipeline links of 1st stage following deferred lighting stages
            if (m_renderPipeline.size () > m_lightingShaderStagesCount + 1 + (m_isShadowMapping ? 1 : 0))
            {
                handle_t stageHandle = m_renderPipeline[m_lightingShaderStagesCount + 1 + (m_isShadowMapping ? 1 : 0)];

                IRenderStage& stage = m_renderStageManager.GetByHandle<IRenderStage> (stageHandle);
                stage.SetColorAttachmentsFramebufferLink (m_isShadowMapping ? EPipelineLink::LINK_THIRD : EPipelineLink::LINK_SECOND);
                stage.SetDepthStencilFramebufferLink (EPipelineLink::LINK_CURRENT);
                stage.SetOutputFramebufferLink (EPipelineLink::LINK_CURRENT);
            }
        }
    }
}

void CGLRenderer::Update (PointLight& pointLight)
{
    handle_t objectHandle = pointLight.GetHandle ();
    size_t lightId;
    size_t uniformBufferOffset;

    // check if light is already in collection
    auto find = m_pointLights.find (objectHandle);

    if (find == m_pointLights.end ())
    {
        lightId = m_uniformPointLightBuffer->pointLightCount++;
        m_pointLights.insert ({ objectHandle, lightId });
    }
    else
    {
        // existing light modified
        lightId = m_pointLights[objectHandle];
    }

    // copy position
    Vector4f lightPosition = pointLight.GetPosition ();
    m_uniformPointLightBuffer->pointLights[lightId].lightPosition[0] = lightPosition[0];
    m_uniformPointLightBuffer->pointLights[lightId].lightPosition[1] = lightPosition[1];
    m_uniformPointLightBuffer->pointLights[lightId].lightPosition[2] = lightPosition[2];

    // copy attenuation factors
    m_uniformPointLightBuffer->pointLights[lightId].attenuationConst = pointLight.GetConstantAttenuationFactor ();
    m_uniformPointLightBuffer->pointLights[lightId].attenuationLinear = pointLight.GetLinearAttenuationFactor ();
    m_uniformPointLightBuffer->pointLights[lightId].attenuationQuadratic = pointLight.GetQuadraticAttenuationFactor ();

    // copy color
    m_uniformPointLightBuffer->pointLights[lightId].lightColor[0] = pointLight.GetColor ()[0];
    m_uniformPointLightBuffer->pointLights[lightId].lightColor[1] = pointLight.GetColor ()[1];
    m_uniformPointLightBuffer->pointLights[lightId].lightColor[2] = pointLight.GetColor ()[2];

    // copy to GPU memory
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_POINTLIGHTS]);

    // load light counts
    glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (m_uniformPointLightBuffer->pointLightCount), &m_uniformPointLightBuffer->pointLightCount);

    // load uniform buffer for a light at given index
    uniformBufferOffset = sizeof (m_uniformPointLightBuffer->pointLightCount) + 3 * sizeof (GLint) + lightId * sizeof (SGlPointLightStruct);
    glBufferSubData (GL_UNIFORM_BUFFER, uniformBufferOffset, sizeof (SGlPointLightStruct), &m_uniformPointLightBuffer->pointLights[lightId]);

    glBindBuffer (GL_UNIFORM_BUFFER, 0);

}

void CGLRenderer::Update (DirectionalLight& directionalLight)
{
    handle_t objectHandle = directionalLight.GetHandle ();
    size_t lightId;
    size_t uniformBufferOffset;

    // check if light is already in collection
    auto find = m_directionalLights.find (objectHandle);

    if (find == m_directionalLights.end ())
    {
        lightId = m_uniformDirectionalLightBuffer->directionalLightCount++;
        m_directionalLights.insert ({ objectHandle, lightId });
    }
    else
    {
        // existing light modified
        lightId = m_directionalLights[objectHandle];
    }

    // update invocation count in shadow map geometry shader
    CGLShader shadowmapShader = CGame::GetResourceManager ().GetByName<CGLShader> ("shadowmap_directional_geometry_shader");
    shadowmapShader.SetParameter ("%%ACTIVE_DIRECTIONAL_LIGHTS%%", std::to_string (GetDirectionalLightCount ()));
    shadowmapShader.Compile ();

    CGLShaderProgram shadowmapShaderProg = GetShaderProgramManager ().GetByName<CGLShaderProgram> ("shadowmap_directional_shader");
    shadowmapShaderProg.Link ();    
    shadowmapShaderProg.BindUniformBlock ("UniformDirectionalLightViewMatricesBlock", EBindingPoint::BP_LIGHTVIEW_DIRECTIONAL);

    // copy direction
    Vector3f lightDirection = directionalLight.GetForward ();
    m_uniformDirectionalLightBuffer->directionalLights[lightId].lightDirection[0] = lightDirection[0];
    m_uniformDirectionalLightBuffer->directionalLights[lightId].lightDirection[1] = lightDirection[1];
    m_uniformDirectionalLightBuffer->directionalLights[lightId].lightDirection[2] = lightDirection[2];

    // copy color
    m_uniformDirectionalLightBuffer->directionalLights[lightId].lightColor[0] = directionalLight.GetColor ()[0];
    m_uniformDirectionalLightBuffer->directionalLights[lightId].lightColor[1] = directionalLight.GetColor ()[1];
    m_uniformDirectionalLightBuffer->directionalLights[lightId].lightColor[2] = directionalLight.GetColor ()[2];

    // copy to GPU memory
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTS]);

    // load light counts
    glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (m_uniformDirectionalLightBuffer->directionalLightCount), &m_uniformDirectionalLightBuffer->directionalLightCount);

    // load uniform buffer for a light at given index
    uniformBufferOffset = sizeof (m_uniformDirectionalLightBuffer->directionalLightCount) + 3 * sizeof (GLint) + lightId * sizeof (SGlDirectionalLightStruct);
    glBufferSubData (GL_UNIFORM_BUFFER, uniformBufferOffset, sizeof (SGlDirectionalLightStruct), &m_uniformDirectionalLightBuffer->directionalLights[lightId]);

    glBindBuffer (GL_UNIFORM_BUFFER, 0);
}

void CGLRenderer::Update (SpotLight& spotLight)
{
    handle_t objectHandle = spotLight.GetHandle ();
    size_t lightId;
    size_t uniformBufferOffset;

    // check if light is already in collection
    auto find = m_spotLights.find (objectHandle);

    if (find == m_spotLights.end ())
    {
        lightId = m_uniformSpotLightBuffer->spotLightCount++;
        m_spotLights.insert ({ objectHandle, lightId });
    }
    else
    {
        // existing light modified
        lightId = m_spotLights[objectHandle];
    }

    // copy position
    Vector4f lightPosition = spotLight.GetPosition ();
    m_uniformSpotLightBuffer->spotLights[lightId].lightPosition[0] = lightPosition[0];
    m_uniformSpotLightBuffer->spotLights[lightId].lightPosition[1] = lightPosition[1];
    m_uniformSpotLightBuffer->spotLights[lightId].lightPosition[2] = lightPosition[2];

    // copy direction
    Vector3f lightDirection = spotLight.GetForward ();
    m_uniformSpotLightBuffer->spotLights[lightId].lightDirection[0] = lightDirection[0];
    m_uniformSpotLightBuffer->spotLights[lightId].lightDirection[1] = lightDirection[1];
    m_uniformSpotLightBuffer->spotLights[lightId].lightDirection[2] = lightDirection[2];
    
    // copy attenuation factors
    m_uniformSpotLightBuffer->spotLights[lightId].attenuationConst = spotLight.GetConstantAttenuationFactor ();
    m_uniformSpotLightBuffer->spotLights[lightId].attenuationLinear = spotLight.GetLinearAttenuationFactor ();
    m_uniformSpotLightBuffer->spotLights[lightId].attenuationQuadratic = spotLight.GetQuadraticAttenuationFactor ();

    // copy cutoff angles
    m_uniformSpotLightBuffer->spotLights[lightId].innerCutoffCosine = std::cos (Mathf::Deg2Rad (spotLight.GetInnerCutoff ()));
    m_uniformSpotLightBuffer->spotLights[lightId].outerCutoffCosine = std::cos (Mathf::Deg2Rad (spotLight.GetOuterCutoff ()));

    // copy color
    m_uniformSpotLightBuffer->spotLights[lightId].lightColor[0] = spotLight.GetColor ()[0];
    m_uniformSpotLightBuffer->spotLights[lightId].lightColor[1] = spotLight.GetColor ()[1];
    m_uniformSpotLightBuffer->spotLights[lightId].lightColor[2] = spotLight.GetColor ()[2];

    // copy to GPU memory
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_SPOTLIGHTS]);

    // load light counts
    glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (m_uniformSpotLightBuffer->spotLightCount), &m_uniformSpotLightBuffer->spotLightCount);

    // load uniform buffer for a light at given index
    uniformBufferOffset = sizeof (m_uniformSpotLightBuffer->spotLightCount) + 3 * sizeof (GLint) + lightId * sizeof (SGlSpotLightStruct);
    glBufferSubData (GL_UNIFORM_BUFFER, uniformBufferOffset, sizeof (SGlSpotLightStruct), &m_uniformSpotLightBuffer->spotLights[lightId]);

    glBindBuffer (GL_UNIFORM_BUFFER, 0);
}

void CGLRenderer::UpdateCameraBuffers (Camera& camera)
{
    LoadViewProjectionUniformBuffers (&camera);
}

void CGLRenderer::UpdateLightViewBuffers ()
{
    LoadLightViewUniformBuffers ();
}

size_t CGLRenderer::GetPointLightCount () const
{
    return m_uniformPointLightBuffer->pointLightCount;
}

size_t CGLRenderer::GetDirectionalLightCount () const
{
    return m_uniformDirectionalLightBuffer->directionalLightCount;
}

size_t CGLRenderer::GetSpotLightCount () const
{
    return m_uniformSpotLightBuffer->spotLightCount;
}

IFramebuffer* CGLRenderer::CreateFramebuffer (unsigned int width, unsigned int height, unsigned int rgbTextureCount, unsigned int rgbaTextureCount, unsigned int depthBufferArrayTextureCount, bool depthStencilRenderbufferEnabled, bool multisampleEnabled)
{
    IFramebuffer* framebuffer;

    if (multisampleEnabled)
    {
#if (CILANTRO_GL_VERSION <= 150)
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "OpenGL 3.2 required for multisample framebuffers";
#else
        framebuffer = new CGLMultisampleFramebuffer (width, height, rgbTextureCount, rgbaTextureCount, depthBufferArrayTextureCount, depthStencilRenderbufferEnabled);
#endif
    }
    else
    {
        framebuffer = new CGLFramebuffer (width, height, rgbTextureCount, rgbaTextureCount, depthBufferArrayTextureCount, depthStencilRenderbufferEnabled);
    }
    
    framebuffer->Initialize ();

    return framebuffer;
}

void CGLRenderer::BindDefaultFramebuffer ()
{
    glBindFramebuffer (GL_FRAMEBUFFER, (GLint) 0);
}

void CGLRenderer::BindDefaultDepthBuffer ()
{
    glFramebufferTexture (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
}

void CGLRenderer::BindDefaultStencilBuffer ()
{
    glFramebufferTexture (GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, 0, 0);
}

void CGLRenderer::BindDefaultTextures ()
{
    for (unsigned int i = 0; i < CILANTRO_MAX_TEXTURE_UNITS; ++i)
    {
        glActiveTexture (GL_TEXTURE0 + i);
        glBindTexture (GL_TEXTURE_2D, 0);
    }
}

void CGLRenderer::ClearColorBuffer (const Vector4f& rgba)
{
    glClearColor (rgba[0], rgba[1], rgba[2], rgba[3]);
    glClear (GL_COLOR_BUFFER_BIT);
}

void CGLRenderer::ClearDepthBuffer ()
{
    glClearDepth (1.0f);
    glClear (GL_DEPTH_BUFFER_BIT);
}

void CGLRenderer::ClearStencilBuffer ()
{
    glClearStencil (0);
    glClear (GL_STENCIL_BUFFER_BIT);
}

void CGLRenderer::SetDepthTestEnabled (bool value)
{
    if (value == true)
    {
        glEnable (GL_DEPTH_TEST);
    }
    else
    {   
        glDisable (GL_DEPTH_TEST);
    }    
}

void CGLRenderer::SetFaceCullingEnabled (bool value)
{
    if (value == true)
    {        
        glEnable (GL_CULL_FACE);
    }
    else
    {   
        glDisable (GL_CULL_FACE);
    }    
}

void CGLRenderer::SetFaceCullingMode (EFaceCullingFace face, EFaceCullingDirection direction)
{
    if (face == EFaceCullingFace::FACE_FRONT)
    {
        glCullFace (GL_FRONT);
    }
    else 
    {
        glCullFace (GL_BACK);
    }

    if (direction == EFaceCullingDirection::DIR_CW)
    {        
        glFrontFace (GL_CW);
    }
    else
    {   
        glFrontFace (GL_CCW);
    }    
}

void CGLRenderer::SetMultisamplingEnabled (bool value)
{
    if (value == true)
    {        
        glEnable (GL_MULTISAMPLE);
    }
    else
    {   
        glDisable (GL_MULTISAMPLE);
    }    
}

void CGLRenderer::SetStencilTestEnabled (bool value)
{
    if (value == true)
    {        
        glEnable (GL_STENCIL_TEST);
        glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
    }
    else
    {   
        glDisable (GL_STENCIL_TEST);
    } 
}

void CGLRenderer::SetStencilTestFunction (EStencilTestFunction testFunction, int testValue)
{
    auto GLFun = [](EStencilTestFunction f)
    {
        switch (f)   
        {
            case EStencilTestFunction::FUNCTION_ALWAYS: return GL_ALWAYS; break;
            case EStencilTestFunction::FUNCTION_EQUAL: return GL_EQUAL; break;
            case EStencilTestFunction::FUNCTION_GEQUAL: return GL_GEQUAL; break;
            case EStencilTestFunction::FUNCTION_GREATER: return GL_GREATER; break;
            case EStencilTestFunction::FUNCTION_LEQUAL: return GL_LEQUAL; break;
            case EStencilTestFunction::FUNCTION_LESS: return GL_LESS; break;
            case EStencilTestFunction::FUNCTION_NEVER: return GL_NEVER; break;
            case EStencilTestFunction::FUNCTION_NOTEQUAL: return GL_NOTEQUAL; break;
            default: return GL_ALWAYS; break;
        }
    };

    glStencilFunc (GLFun (testFunction), testValue, 0xff);
    glStencilMask (0xff);
}

void CGLRenderer::SetStencilTestOperation (EStencilTestOperation sFail, EStencilTestOperation dpFail, EStencilTestOperation dpPass)
{
    auto GLOp = [](EStencilTestOperation op)
    {
        switch (op)
        {
            case EStencilTestOperation::OP_KEEP: return GL_KEEP; break;
            case EStencilTestOperation::OP_REPLACE: return GL_REPLACE; break;
            case EStencilTestOperation::OP_ZERO: return GL_ZERO; break;
            case EStencilTestOperation::OP_INC: return GL_INCR; break;
            case EStencilTestOperation::OP_DEC: return GL_DECR; break;
            case EStencilTestOperation::OP_INC_WRAP: return GL_INCR_WRAP; break;
            case EStencilTestOperation::OP_DEC_WRAP: return GL_DECR_WRAP; break;
            case EStencilTestOperation::OP_INV: return GL_INVERT; break;
            default: return GL_KEEP; break;
        }
    };

    glStencilOp (GLOp (sFail), GLOp (dpFail), GLOp (dpPass));
}

void CGLRenderer::InitializeShaderLibrary ()
{
    CGLShaderProgram* p;

    // load standard shaders
    CGame::GetResourceManager ().Load<CGLShader> ("default_vertex_shader", "shaders/default.vs", EShaderType::VERTEX_SHADER);
    CGame::GetResourceManager ().Load<CGLShader> ("flatquad_vertex_shader", "shaders/flatquad.vs", EShaderType::VERTEX_SHADER);
    CGame::GetResourceManager ().Load<CGLShader> ("pbr_forward_fragment_shader", "shaders/pbr_forward.fs", EShaderType::FRAGMENT_SHADER);    
    CGame::GetResourceManager ().Load<CGLShader> ("pbr_deferred_geometrypass_fragment_shader", "shaders/pbr_deferred_geometrypass.fs", EShaderType::FRAGMENT_SHADER); 
    CGame::GetResourceManager ().Load<CGLShader> ("pbr_deferred_lightingpass_fragment_shader", "shaders/pbr_deferred_lightingpass.fs", EShaderType::FRAGMENT_SHADER); 
    CGame::GetResourceManager ().Load<CGLShader> ("blinnphong_forward_fragment_shader", "shaders/blinnphong_forward.fs", EShaderType::FRAGMENT_SHADER);
    CGame::GetResourceManager ().Load<CGLShader> ("blinnphong_deferred_geometrypass_fragment_shader", "shaders/blinnphong_deferred_geometrypass.fs", EShaderType::FRAGMENT_SHADER); 
    CGame::GetResourceManager ().Load<CGLShader> ("blinnphong_deferred_lightingpass_fragment_shader", "shaders/blinnphong_deferred_lightingpass.fs", EShaderType::FRAGMENT_SHADER); 
    CGame::GetResourceManager ().Load<CGLShader> ("flatquad_fragment_shader", "shaders/flatquad.fs", EShaderType::FRAGMENT_SHADER);
    CGame::GetResourceManager ().Load<CGLShader> ("post_hdr_fragment_shader", "shaders/post_hdr.fs", EShaderType::FRAGMENT_SHADER);
    CGame::GetResourceManager ().Load<CGLShader> ("post_gamma_fragment_shader", "shaders/post_gamma.fs", EShaderType::FRAGMENT_SHADER);
    CGame::GetResourceManager ().Load<CGLShader> ("post_fxaa_fragment_shader", "shaders/post_fxaa.fs", EShaderType::FRAGMENT_SHADER);
    CGame::GetResourceManager ().Load<CGLShader> ("shadowmap_vertex_shader", "shaders/shadowmap.vs", EShaderType::VERTEX_SHADER);
    CGame::GetResourceManager ().Load<CGLShader> ("shadowmap_directional_geometry_shader", "shaders/shadowmap_directional.gs", EShaderType::GEOMETRY_SHADER);
    CGame::GetResourceManager ().Load<CGLShader> ("shadowmap_fragment_shader", "shaders/shadowmap.fs", EShaderType::FRAGMENT_SHADER);

    // PBR model (forward)
    p = &AddShaderProgram<CGLShaderProgram> ("pbr_forward_shader");
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("default_vertex_shader"));
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("pbr_forward_fragment_shader"));
    p->Link ();
    p->Use ();
#if (CILANTRO_GL_VERSION < 330)
    glBindAttribLocation(p->GetProgramId (), 0, "vPosition");
    glBindAttribLocation(p->GetProgramId (), 1, "vNormal");
    glBindAttribLocation(p->GetProgramId (), 2, "vUV");
    glBindAttribLocation(p->GetProgramId (), 3, "vTangent");
    glBindAttribLocation(p->GetProgramId (), 4, "vBitangent");
#endif
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tAlbedo"), 0);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tNormal"), 1);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tMetallic"), 2);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tRoughness"), 3);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tAO"), 4);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tDirectionalShadowMap"), 5);
#endif
    p->BindUniformBlock ("UniformMatricesBlock", EBindingPoint::BP_MATRICES);
    p->BindUniformBlock ("UniformPointLightsBlock", EBindingPoint::BP_POINTLIGHTS);
    p->BindUniformBlock ("UniformDirectionalLightsBlock", EBindingPoint::BP_DIRECTIONALLIGHTS);
    p->BindUniformBlock ("UniformSpotLightsBlock", EBindingPoint::BP_SPOTLIGHTS);
    p->BindUniformBlock ("UniformDirectionalLightViewMatricesBlock", EBindingPoint::BP_LIGHTVIEW_DIRECTIONAL);
    CheckGLError (MSG_LOCATION);

    // PBR model (deferred, geometry pass)
    p = &AddShaderProgram<CGLShaderProgram> ("pbr_deferred_geometrypass_shader");
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("default_vertex_shader"));
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("pbr_deferred_geometrypass_fragment_shader"));
    p->Link ();
    p->Use ();
#if (CILANTRO_GL_VERSION < 330)
    glBindAttribLocation (p->GetProgramId (), 0, "vPosition");
    glBindAttribLocation (p->GetProgramId (), 1, "vNormal");
    glBindAttribLocation (p->GetProgramId (), 2, "vUV");
    glBindAttribLocation (p->GetProgramId (), 3, "vTangent");
    glBindAttribLocation (p->GetProgramId (), 4, "vBitangent");
#endif
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tAlbedo"), 0);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tNormal"), 1);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tMetallic"), 2);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tRoughness"), 3);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tAO"), 4);
#endif
    p->BindUniformBlock ("UniformMatricesBlock", EBindingPoint::BP_MATRICES);
    CheckGLError (MSG_LOCATION);

    // PBR model (deferred, lighting pass)
    p = &AddShaderProgram<CGLShaderProgram> ("pbr_deferred_lightingpass_shader");
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("flatquad_vertex_shader"));
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("pbr_deferred_lightingpass_fragment_shader"));
    p->Link ();
    p->Use ();
#if (CILANTRO_GL_VERSION < 330)
    glBindAttribLocation(p->GetProgramId (), 0, "vPosition");
    glBindAttribLocation(p->GetProgramId (), 1, "vTextureCoordinates");
#endif
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tPosition"), 0);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tNormal"), 1);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tAlbedo"), 2);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tMetallicRoughnessAO"), 3);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tUnused"), 4);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tDirectionalShadowMap"), 5);
#endif
    p->BindUniformBlock ("UniformPointLightsBlock", EBindingPoint::BP_POINTLIGHTS);
    p->BindUniformBlock ("UniformDirectionalLightsBlock", EBindingPoint::BP_DIRECTIONALLIGHTS);
    p->BindUniformBlock ("UniformSpotLightsBlock", EBindingPoint::BP_SPOTLIGHTS);
    p->BindUniformBlock ("UniformDirectionalLightViewMatricesBlock", EBindingPoint::BP_LIGHTVIEW_DIRECTIONAL);  
    CheckGLError (MSG_LOCATION);

    // Blinn-Phong model (forward)
    p = &AddShaderProgram<CGLShaderProgram> ("blinnphong_forward_shader");
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("default_vertex_shader"));
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("blinnphong_forward_fragment_shader"));
    p->Link ();
    p->Use ();
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation(p->GetProgramId (), 0, "vPosition");
    glBindAttribLocation(p->GetProgramId (), 1, "vNormal");
    glBindAttribLocation(p->GetProgramId (), 2, "vUV");
    glBindAttribLocation(p->GetProgramId (), 3, "vTangent");
    glBindAttribLocation(p->GetProgramId (), 4, "vBitangent");
#endif
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tDiffuse"), 0);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tNormal"), 1);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tSpecular"), 2);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tEmissive"), 3);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tDirectionalShadowMap"), 4);
#endif
    p->BindUniformBlock ("UniformMatricesBlock", EBindingPoint::BP_MATRICES);
    p->BindUniformBlock ("UniformPointLightsBlock", EBindingPoint::BP_POINTLIGHTS);
    p->BindUniformBlock ("UniformDirectionalLightsBlock", EBindingPoint::BP_DIRECTIONALLIGHTS);
    p->BindUniformBlock ("UniformSpotLightsBlock", EBindingPoint::BP_SPOTLIGHTS); 
    p->BindUniformBlock ("UniformDirectionalLightViewMatricesBlock", EBindingPoint::BP_LIGHTVIEW_DIRECTIONAL);
    CheckGLError (MSG_LOCATION);
    
    // Blinn-Phong model (deferred, geometry pass)
    p = &AddShaderProgram<CGLShaderProgram> ("blinnphong_deferred_geometrypass_shader");
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("default_vertex_shader"));
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("blinnphong_deferred_geometrypass_fragment_shader"));
    p->Link ();
    p->Use ();
#if (CILANTRO_GL_VERSION < 330)
    glBindAttribLocation(p->GetProgramId (), 0, "vPosition");
    glBindAttribLocation(p->GetProgramId (), 1, "vNormal");
    glBindAttribLocation(p->GetProgramId (), 2, "vUV");
    glBindAttribLocation(p->GetProgramId (), 3, "vTangent");
    glBindAttribLocation(p->GetProgramId (), 4, "vBitangent");
#endif
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tDiffuse"), 0);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tNormal"), 1);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tSpecular"), 2);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tEmissive"), 3);
#endif    
    p->BindUniformBlock ("UniformMatricesBlock", EBindingPoint::BP_MATRICES);
    CheckGLError (MSG_LOCATION);

    // Blinn-Phong model (deferred, lighting pass)
    p = &AddShaderProgram<CGLShaderProgram> ("blinnphong_deferred_lightingpass_shader");
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("flatquad_vertex_shader"));
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("blinnphong_deferred_lightingpass_fragment_shader"));
    p->Link ();
    p->Use ();
#if (CILANTRO_GL_VERSION < 330)
    glBindAttribLocation(p->GetProgramId (), 0, "vPosition");
    glBindAttribLocation(p->GetProgramId (), 1, "vTextureCoordinates");
#endif
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tPosition"), 0);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tNormal"), 1);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tDiffuse"), 2);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tEmissive"), 3);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tSpecular"), 4);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tDirectionalShadowMap"), 5);
#endif
    p->BindUniformBlock ("UniformPointLightsBlock", EBindingPoint::BP_POINTLIGHTS);
    p->BindUniformBlock ("UniformDirectionalLightsBlock", EBindingPoint::BP_DIRECTIONALLIGHTS);
    p->BindUniformBlock ("UniformSpotLightsBlock", EBindingPoint::BP_SPOTLIGHTS);   
    p->BindUniformBlock ("UniformDirectionalLightViewMatricesBlock", EBindingPoint::BP_LIGHTVIEW_DIRECTIONAL); 
    CheckGLError (MSG_LOCATION);

    // Screen quad rendering
    p = &AddShaderProgram<CGLShaderProgram> ("flatquad_shader");
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("flatquad_vertex_shader"));
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("flatquad_fragment_shader"));   
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation (p->GetProgramId (), 0, "vPosition");
    glBindAttribLocation (p->GetProgramId (), 1, "vTextureCoordinates");
#endif
    p->Link ();
    p->Use (); 
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "fScreenTexture"), 0);
#endif
    CheckGLError (MSG_LOCATION);

    // Post-processing HDR
    p = &AddShaderProgram<CGLShaderProgram> ("post_hdr_shader");
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("flatquad_vertex_shader"));
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("post_hdr_fragment_shader"));
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation (p->GetProgramId (), 0, "vPosition");
    glBindAttribLocation (p->GetProgramId (), 1, "vTextureCoordinates");
#endif
    p->Link ();
    p->Use ();
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "fScreenTexture"), 0);
#endif
    CheckGLError (MSG_LOCATION);

    // Post-processing gamma
    p = &AddShaderProgram<CGLShaderProgram> ("post_gamma_shader");
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("flatquad_vertex_shader"));
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("post_gamma_fragment_shader"));   
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation (p->GetProgramId (), 0, "vPosition");
    glBindAttribLocation (p->GetProgramId (), 1, "vTextureCoordinates");
#endif
    p->Link ();
    p->Use (); 
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "fScreenTexture"), 0);
#endif
    CheckGLError (MSG_LOCATION);

    // Post-processing fxaa
    p = &AddShaderProgram<CGLShaderProgram> ("post_fxaa_shader");
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("flatquad_vertex_shader"));
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("post_fxaa_fragment_shader"));   
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation (p->GetProgramId (), 0, "vPosition");
    glBindAttribLocation (p->GetProgramId (), 1, "vTextureCoordinates");
#endif
    p->Link ();
    p->Use (); 
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "fScreenTexture"), 0);
#endif
    CheckGLError (MSG_LOCATION);

    // Shadow map (directional)
    p = &AddShaderProgram<CGLShaderProgram> ("shadowmap_directional_shader");
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("shadowmap_vertex_shader"));
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("shadowmap_directional_geometry_shader"));
    p->AttachShader (CGame::GetResourceManager ().GetByName<CGLShader>("shadowmap_fragment_shader"));
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation (p->GetProgramId (), 0, "vPosition");
#endif
    p->Link ();
    p->Use (); 
#if (CILANTRO_GL_VERSION < 420)
#endif
    p->BindUniformBlock ("UniformDirectionalLightViewMatricesBlock", EBindingPoint::BP_LIGHTVIEW_DIRECTIONAL);
    CheckGLError (MSG_LOCATION);

}

void CGLRenderer::InitializeMatrixUniformBuffers ()
{
    // create uniform buffer for view and projection matrices
    glGenBuffers (1, &m_uniformBuffers->UBO[UBO_MATRICES]);
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_MATRICES]);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGlUniformMatrixBuffer), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, EBindingPoint::BP_MATRICES, m_uniformBuffers->UBO[UBO_MATRICES]);

    // create unform buffers for light view transforms
    glGenBuffers (1, &m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTVIEWMATRICES]);
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTVIEWMATRICES]);
    glBufferData (GL_UNIFORM_BUFFER, 16 * sizeof (GLfloat) * CILANTRO_MAX_DIRECTIONAL_LIGHTS, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, EBindingPoint::BP_LIGHTVIEW_DIRECTIONAL, m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTVIEWMATRICES]);

    CheckGLError (MSG_LOCATION);
}

void CGLRenderer::LoadViewProjectionUniformBuffers (Camera* camera)
{
    Matrix4f view = camera->GetViewMatrix ();
    Matrix4f projection = camera->GetProjectionMatrix (m_width, m_height);

    // load view matrix
    std::memcpy (m_uniformMatrixBuffer->viewMatrix, Mathf::Transpose (view)[0], 16 * sizeof (GLfloat));

    // load projection matrix
    std::memcpy (m_uniformMatrixBuffer->projectionMatrix, Mathf::Transpose (projection)[0], 16 * sizeof (GLfloat));

    // load to GPU - view and projection
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_MATRICES]);
    glBufferSubData (GL_UNIFORM_BUFFER, 0, 16 * sizeof (GLfloat), m_uniformMatrixBuffer->viewMatrix);
    glBufferSubData (GL_UNIFORM_BUFFER, 16 * sizeof (GLfloat), 16 * sizeof (GLfloat), m_uniformMatrixBuffer->projectionMatrix);
    glBindBuffer (GL_UNIFORM_BUFFER, 0);
}

void CGLRenderer::LoadLightViewUniformBuffers ()
{
    Matrix4f view = m_gameScene->GetActiveCamera ()->GetViewMatrix ();
    Matrix4f projection = m_gameScene->GetActiveCamera ()->GetProjectionMatrix (m_width, m_height);
    Matrix4f invMV = Mathf::Invert (projection * view);
    Matrix4f lightView;
    Matrix4f lightViewProjection;
    Vector3f frustumCenter {0.0f, 0.0f, 0.0f};
    std::vector<Vector4f> frustumVertices;

    // calculate camera frustum vertices in world space
    for (size_t x = 0; x < 2; ++x)
    {
        for (size_t y = 0; y < 2; ++y)
        {
            for (size_t z = 0; z < 2; ++z)
            {
                Vector4f v = invMV * Vector4f {2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f};

                frustumVertices.push_back (v / v[3]);
            }
        }
    }

    // find frustum center
    for (auto&& v : frustumVertices)
    {
        frustumCenter += v;
    }
    frustumCenter /= (float) frustumVertices.size ();

    // calculate and load lightview matrix for each directional light
    for (auto&& light : m_directionalLights)
    {
        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::min();

        size_t lightIdx = light.second;
        DirectionalLight& l = m_gameScene->GetGameObjectManager ().GetByHandle<DirectionalLight> (light.first);
        lightView = Mathf::GenCameraViewMatrix (frustumCenter - l.GetForward (), frustumCenter, l.GetUp ());

        for (auto&& v : frustumVertices)
        {
            Vector4f lv = lightView * v;

            minX = std::min (minX, lv[0]);
            maxX = std::max (maxX, lv[0]);
            minY = std::min (minY, lv[1]);
            maxY = std::max (maxY, lv[1]);
            minZ = std::min (minZ, lv[2]);
            maxZ = std::max (maxZ, lv[2]);
        }

        lightViewProjection = Mathf::GenOrthographicProjectionMatrix (minX, maxX, minY, maxY, minZ, maxZ) * lightView;

        // copy to buffer
        std::memcpy (m_uniformMatrixBuffer->directionalLightView + lightIdx * 16, Mathf::Transpose (lightViewProjection)[0], 16 * sizeof (GLfloat));
    }

    // load to GPU - directional light view
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTVIEWMATRICES]);
    glBufferSubData (GL_UNIFORM_BUFFER, 0, 16 * sizeof (GLfloat) * m_uniformDirectionalLightBuffer->directionalLightCount, m_uniformMatrixBuffer->directionalLightView);
    glBindBuffer (GL_UNIFORM_BUFFER, 0);

}

void CGLRenderer::DeinitializeMatrixUniformBuffers ()
{
    glDeleteBuffers (1, &m_uniformBuffers->UBO[UBO_MATRICES]);
    glDeleteBuffers (1, &m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTVIEWMATRICES]);
}

void CGLRenderer::InitializeObjectBuffers ()
{
    // create and load object buffers for all existing objects
    for (auto&& gameObject : m_gameScene->GetGameObjectManager ())
    {
        // load buffers for MeshObject only
        if (std::dynamic_pointer_cast<MeshObject> (gameObject) != nullptr)
        {
            gameObject->OnUpdate (*this);
        }      
    }
}

void CGLRenderer::DeinitializeObjectBuffers ()
{
    for (auto&& buffer : m_sceneGeometryBuffers)
    {
        glDeleteBuffers (CILANTRO_VBO_COUNT, buffer.second->VBO);
        glDeleteBuffers (1, &buffer.second->EBO);
        glDeleteVertexArrays (1, &buffer.second->VAO);
    }
}

void CGLRenderer::InitializeQuadGeometryBuffer ()
{
    // set up VBO and VAO
    float quadVertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    float quadUV[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    };

    GLuint quadIndices[] = {
        0, 1, 2,
        2, 1, 3
    };

    glGenVertexArrays (1, &m_quadGeometryBuffer->VAO);    
    glBindVertexArray (m_quadGeometryBuffer->VAO);

    glGenBuffers (1, &m_quadGeometryBuffer->VBO[EGlVboType::VBO_VERTICES]);
    glGenBuffers (1, &m_quadGeometryBuffer->VBO[EGlVboType::VBO_UVS]);

    glBindBuffer (GL_ARRAY_BUFFER, m_quadGeometryBuffer->VBO[EGlVboType::VBO_VERTICES]);
    glBufferData (GL_ARRAY_BUFFER, sizeof (quadVertices), &quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer (EGlVboType::VBO_VERTICES, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer (GL_ARRAY_BUFFER, m_quadGeometryBuffer->VBO[EGlVboType::VBO_UVS]);
    glBufferData (GL_ARRAY_BUFFER, sizeof (quadUV), &quadUV, GL_STATIC_DRAW);
    glVertexAttribPointer (EGlVboType::VBO_UVS, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glEnableVertexAttribArray (EGlVboType::VBO_VERTICES);
    glEnableVertexAttribArray (EGlVboType::VBO_UVS);

    glGenBuffers (1, &m_quadGeometryBuffer->EBO);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_quadGeometryBuffer->EBO);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (quadIndices), &quadIndices, GL_STATIC_DRAW);

    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindVertexArray (0);    

    m_quadGeometryBuffer->indexCount = 6;

    CheckGLError (MSG_LOCATION);
}

void CGLRenderer::DeinitializeQuadGeometryBuffer ()
{
    glDeleteVertexArrays(1, &m_quadGeometryBuffer->VAO);
    glDeleteBuffers(1, &m_quadGeometryBuffer->VBO[EGlVboType::VBO_VERTICES]);
}

void CGLRenderer::InitializeLightUniformBuffers ()
{
    m_uniformPointLightBuffer->pointLightCount = 0;
    m_uniformSpotLightBuffer->spotLightCount = 0;
    m_uniformDirectionalLightBuffer->directionalLightCount = 0;

    // create uniform buffer for point lights
    glGenBuffers (1, &m_uniformBuffers->UBO[UBO_POINTLIGHTS]);
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_POINTLIGHTS]);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGlUniformPointLightBuffer), m_uniformPointLightBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, EBindingPoint::BP_POINTLIGHTS, m_uniformBuffers->UBO[UBO_POINTLIGHTS]);

    // create uniform buffer for directional lights
    glGenBuffers (1, &m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTS]);
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTS]);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGlUniformDirectionalLightBuffer), m_uniformDirectionalLightBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, EBindingPoint::BP_DIRECTIONALLIGHTS, m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTS]);

    // create uniform buffer for spot lights
    glGenBuffers (1, &m_uniformBuffers->UBO[UBO_SPOTLIGHTS]);
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_SPOTLIGHTS]);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGlUniformSpotLightBuffer), m_uniformSpotLightBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, EBindingPoint::BP_SPOTLIGHTS, m_uniformBuffers->UBO[UBO_SPOTLIGHTS]);

    // scan objects vector for lights and populate light buffers
    for (auto gameObject : m_gameScene->GetGameObjectManager ())
    {
        if (std::dynamic_pointer_cast<Light> (gameObject) != nullptr)
        {
            gameObject->OnUpdate (*this);
        }
    }

}

void CGLRenderer::DeinitializeLightUniformBuffers ()
{
    // delete all light buffers
    glDeleteBuffers (1, &m_uniformBuffers->UBO[UBO_POINTLIGHTS]);
    glDeleteBuffers (1, &m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTS]);
    glDeleteBuffers (1, &m_uniformBuffers->UBO[UBO_SPOTLIGHTS]);
}

void CGLRenderer::UpdateLightBufferRecursive (handle_t objectHandle)
{
    GameObject* light = &m_gameScene->GetGameObjectManager ().GetByHandle<GameObject> (objectHandle);

    if (dynamic_cast<Light*>(light) != nullptr)
    {
        light->OnUpdate (*this);
    }

    for (auto& childObject : light->GetChildObjects ())
    {
        UpdateLightBufferRecursive (childObject->GetHandle ());
    }

}

void CGLRenderer::RenderGeometryBuffer (SGlGeometryBuffers* buffer)
{
    // bind
    glBindVertexArray (buffer->VAO);
    
    // draw
    glDrawElements (GL_TRIANGLES, static_cast<GLsizei> (buffer->indexCount) * sizeof (GLuint), GL_UNSIGNED_INT, 0);
    
    // unbind
    glBindVertexArray (0);
}

void CGLRenderer::CheckGLError (const std::string& location)
{
    GLuint errorCode;

    if ((errorCode = glGetError ()) != GL_NO_ERROR)
    {
        LogMessage (location, EXIT_FAILURE) << "glError:" << std::hex << std::showbase << errorCode;
    }
}
