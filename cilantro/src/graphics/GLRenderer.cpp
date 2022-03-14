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

CGLRenderer::CGLRenderer (CGameScene* gameScene, unsigned int width, unsigned int height, bool isDeferred) 
    : CRenderer (gameScene, width, height, isDeferred)
{
    m_QuadGeometryBuffer = new SGlGeometryBuffers ();
    m_UniformBuffers = new SGlUniformBuffers ();
    m_UniformMatrixBuffer = new SGlUniformMatrixBuffer ();
    m_UniformPointLightBuffer = new SGlUniformPointLightBuffer ();
    m_UniformDirectionalLightBuffer = new SGlUniformDirectionalLightBuffer ();
    m_UniformSpotLightBuffer = new SGlUniformSpotLightBuffer ();
}

CGLRenderer::~CGLRenderer ()
{
    for (auto&& objectBuffer : m_SceneGeometryBuffers)
    {
        delete objectBuffer.second;
    }

    delete m_QuadGeometryBuffer;
    delete m_UniformBuffers;
    delete m_UniformMatrixBuffer;
    delete m_UniformPointLightBuffer;
    delete m_UniformDirectionalLightBuffer;
    delete m_UniformSpotLightBuffer;
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
    m_gameScene->RegisterCallback ("OnUpdateMeshObject", 
        [&](unsigned int objectHandle, unsigned int) 
        { 
            m_gameScene->GetGameObjectManager ().GetByHandle<GameObject> (objectHandle).OnUpdate (*this); 
        }
    );

    // set callback for new or modified materials
    m_gameScene->RegisterCallback ("OnUpdateMaterialTexture", 
        [&](unsigned int materialHandle, unsigned int textureUnit) 
        { 
            Update (m_gameScene->GetMaterialManager ().GetByHandle<Material> (materialHandle), textureUnit); 
        }
    );

    m_gameScene->RegisterCallback ("OnUpdateMaterial", 
        [&](unsigned int materialHandle, unsigned int) 
        { 
            Update (m_gameScene->GetMaterialManager ().GetByHandle<Material> (materialHandle)); 
        }
    );
    
    // set callback for new or modified lights
    m_gameScene->RegisterCallback ("OnUpdateLight", 
        [&](unsigned int objectHandle, unsigned int) 
        { 
            m_gameScene->GetGameObjectManager ().GetByHandle<GameObject> (objectHandle).OnUpdate (*this); 
        }
    );

    // set callback for modified scene graph (currently this only requires to reload light buffers)
    m_gameScene->RegisterCallback ("OnUpdateSceneGraph", 
        [&](unsigned int objectHandle, unsigned int) 
        { 
            UpdateLightBufferRecursive (objectHandle);
        }
    );

    // set callback for modified transforms (currently this only requires to reload light buffers)
    m_gameScene->RegisterCallback ("OnUpdateTransform", 
        [&](unsigned int objectHandle, unsigned int) 
        { 
            UpdateLightBufferRecursive (objectHandle); 
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
    GLuint eyePositionId;
    GLuint modelMatrixId;
    GLuint normalMatrixId;
    GLuint boneTransformationArrayId;
    GLuint shaderProgramId;
    GLuint uniformId;

    Material& objM = meshObject.GetMaterial ();

    // get shader program for rendered meshobject (geometry pass)
    GLShaderProgram& geometryShaderProgram = m_shaderProgramManager.GetByName<GLShaderProgram> (
        m_isDeferred
        ? objM.GetDeferredGeometryPassShaderProgram ()
        : objM.GetForwardShaderProgram ()
    );
    geometryShaderProgram.Use ();
    shaderProgramId = geometryShaderProgram.GetProgramId ();

    // bind textures for active material
    if (materialTextureUnits.find(meshObject.GetMaterial ().GetHandle ()) != materialTextureUnits.end ())
    {
        SGlMaterialTextureUnits* u = materialTextureUnits[meshObject.GetMaterial ().GetHandle ()];

        for (GLuint i = 0; i < u->unitsCount; i++)
        {
            glActiveTexture (GL_TEXTURE0 + i);
            glBindTexture (GL_TEXTURE_2D, u->textureUnits[i]);
        }
    }
    else
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Missing texture for object" << meshObject.GetHandle ();
    }

    // set material uniforms for active material
    for (auto&& property : meshObject.GetMaterial ().GetPropertiesMap ())
    {
        uniformId = glGetUniformLocation (shaderProgramId, property.first.c_str ());
        if (uniformId != GL_INVALID_INDEX)
        {
            if (property.second.size () == 1)
            {
                glUniform1f (uniformId, property.second[0]);
            }
            else if ((property.second.size () == 3))
            {
                glUniform3fv (uniformId, 1, property.second.data ());
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
    modelMatrixId = glGetUniformLocation (shaderProgramId, "mModel");
    if (modelMatrixId != GL_INVALID_INDEX)
    {
        glUniformMatrix4fv (modelMatrixId, 1, GL_TRUE, meshObject.GetModelTransformMatrix ()[0]);
    }

    // calculate normal matrix for drawn objects and set uniform value
    normalMatrixId = glGetUniformLocation (shaderProgramId, "mNormal");
    if (normalMatrixId != GL_INVALID_INDEX)
    {
        glUniformMatrix3fv (normalMatrixId, 1, GL_TRUE, Mathf::Invert (Mathf::Transpose (Matrix3f (meshObject.GetModelTransformMatrix ())))[0]);
    }

    // get camera position in world space and set uniform value
    eyePositionId = glGetUniformLocation (shaderProgramId, "eyePosition");
    if (eyePositionId != GL_INVALID_INDEX)
    {
        glUniform3fv (eyePositionId, 1, &m_gameScene->GetActiveCamera ()->GetPosition ()[0]);
    }

    // set bone transformation matrix array uniform
    boneTransformationArrayId = glGetUniformLocation (shaderProgramId, "mBoneTransformations");
    if (boneTransformationArrayId != GL_INVALID_INDEX)
    {
        glUniformMatrix4fv (boneTransformationArrayId, CILANTRO_MAX_BONES, GL_TRUE, meshObject.GetBoneTransformationsMatrixArray ());
    }

    // get shader program for rendered meshobject (lighting pass)
    if (m_isDeferred)
    {
        GLShaderProgram& lightingShaderProgram = m_shaderProgramManager.GetByName<GLShaderProgram>(objM.GetDeferredLightingPassShaderProgram ());
        lightingShaderProgram.Use ();
        shaderProgramId = lightingShaderProgram.GetProgramId ();

        // get camera position in world space and set uniform value (this needs to be done again for deferred lighting shader program)
        eyePositionId = glGetUniformLocation (shaderProgramId, "eyePosition");
        if (eyePositionId != GL_INVALID_INDEX)
        {
            glUniform3fv (eyePositionId, 1, &m_gameScene->GetActiveCamera ()->GetPosition ()[0]);
        }

    }
    
    // draw mesh
    SGlGeometryBuffers* b = m_SceneGeometryBuffers[meshObject.GetHandle ()];
    geometryShaderProgram.Use ();
    glBindVertexArray (b->VAO);
    glDrawElements (GL_TRIANGLES, meshObject.GetMesh ().GetIndexCount (), GL_UNSIGNED_INT, 0);
    glBindVertexArray (0);
}

void CGLRenderer::DrawViewportQuad (unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
    glViewport (x, y, width, height);
    RenderGeometryBuffer (m_QuadGeometryBuffer);
}

void CGLRenderer::Update (MeshObject& meshObject)
{
    unsigned int objectHandle = meshObject.GetHandle ();

    // check of object's buffers are already initialized
    auto find = m_SceneGeometryBuffers.find (objectHandle);

    if (find == m_SceneGeometryBuffers.end ())
    {
        // it is a new object, so generate buffers 
        SGlGeometryBuffers* b = new SGlGeometryBuffers ();
        m_SceneGeometryBuffers.insert ({ objectHandle, b });

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
    SGlGeometryBuffers* b = m_SceneGeometryBuffers[objectHandle];

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
    glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (int) * CILANTRO_MAX_BONE_INFLUENCES, meshObject.GetMesh ().GetBoneIndicesData (), GL_STATIC_DRAW);

    // load bone weight buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVboType::VBO_BONEWEIGHTS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * CILANTRO_MAX_BONE_INFLUENCES, meshObject.GetMesh ().GetBoneWeightsData (), GL_STATIC_DRAW);

    // load index buffer
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, b->EBO);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, meshObject.GetMesh ().GetIndexCount () * sizeof (unsigned int), meshObject.GetMesh ().GetFacesData (), GL_STATIC_DRAW);

    // unbind VAO
    glBindVertexArray (0);

}

void CGLRenderer::Update (Material& material, unsigned int textureUnit)
{
    unsigned int materialHandle = material.GetHandle ();
    GLuint texture;
    GLuint format;

    texture_map_t& textures = material.GetTexturesMap ();

    // check if material already exists
    auto find = materialTextureUnits.find (materialHandle);

    if (find == materialTextureUnits.end ())
    {
        materialTextureUnits.insert ({ materialHandle, new SGlMaterialTextureUnits () });
        
        for (auto&& t : textures)
        {
            Texture* tPtr = t.second.second;
            std::string tName = t.second.first;
            GLuint unit = t.first;
            format = GetTextureFormat (tPtr->GetChannels ());

            glGenTextures (1, &texture);
            glBindTexture (GL_TEXTURE_2D, texture);
            glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D (GL_TEXTURE_2D, 0, format, tPtr->GetWidth (), tPtr->GetHeight (), 0, format, GL_UNSIGNED_BYTE, tPtr->Data ());
            glGenerateMipmap (GL_TEXTURE_2D);
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture (GL_TEXTURE_2D, 0);
            
            materialTextureUnits[materialHandle]->textureUnits[unit] = texture;
        }

        materialTextureUnits[materialHandle]->unitsCount = textures.size ();

    }
    else
    {
        auto& t = textures[textureUnit];
        Texture* tPtr = t.second;
        std::string tName = t.first;
        GLuint unit = textureUnit;
        format = GetTextureFormat (tPtr->GetChannels ());

        glBindTexture (GL_TEXTURE_2D, materialTextureUnits[materialHandle]->textureUnits[unit]);
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
    handle_t shaderProgramHandle = m_shaderProgramManager.GetByName<ShaderProgram>(material.GetDeferredLightingPassShaderProgram ()).GetHandle ();
    std::string shaderProgramName = material.GetDeferredLightingPassShaderProgram ();

    if (m_isDeferred)
    {
        // add material's shader program to set of used shader programs handles
        // add lighting deferred pass renderStages for each program
        if (m_lightingShaders.find (shaderProgramHandle) == m_lightingShaders.end ())
        {
            // first rotate the pipeline to the left so that geometry stage is last
            RotateRenderPipelineLeft ();

            m_lightingShaderStagesCount++;
            m_lightingShaders.insert (shaderProgramHandle);
            CQuadRenderStage& p = AddRenderStage <CQuadRenderStage> ("deferredLightingStage_" + shaderProgramName);
            p.SetShaderProgram (shaderProgramName);
            p.SetStencilTestEnabled (true).SetStencilTest (EStencilTestFunction::FUNCTION_EQUAL, shaderProgramHandle);
            p.SetClearColorOnFrameEnabled (true);
            p.SetClearDepthOnFrameEnabled (false);
            p.SetClearStencilOnFrameEnabled (false);
            p.SetDepthTestEnabled (false);
            p.SetPipelineFramebufferInputLink (EPipelineLink::LINK_FIRST);
            p.SetPipelineRenderbufferLink (EPipelineLink::LINK_FIRST);
            p.SetPipelineFramebufferDrawLink (EPipelineLink::LINK_SECOND);
            p.SetFramebufferEnabled (true);

            p.Initialize ();

            // rotate pipeline twice to the right, so that ultimately geometry stage is first and newly added stage is second
            RotateRenderPipelineRight ();
            RotateRenderPipelineRight ();
            
            // update flags of other deferred lighting stages (if present)
            if (m_lightingShaderStagesCount > 1)
            {
                handle_t stageHandle = GetRenderPipeline ()[2];

                CQuadRenderStage& stage = m_renderStageManager.GetByHandle<CQuadRenderStage> (stageHandle);
                stage.SetClearColorOnFrameEnabled (false);
                stage.SetFramebufferEnabled (false);
            }

            // update pipeline links of 1st stage following deferred lighting stages
            if (m_renderPipeline.size () > m_lightingShaderStagesCount + 1)
            {
                handle_t stageHandle = m_renderPipeline[m_lightingShaderStagesCount + 1];

                IRenderStage& stage = m_renderStageManager.GetByHandle<IRenderStage> (stageHandle);
                stage.SetPipelineFramebufferInputLink (EPipelineLink::LINK_SECOND);
                stage.SetPipelineRenderbufferLink (EPipelineLink::LINK_CURRENT);
                stage.SetPipelineFramebufferDrawLink (EPipelineLink::LINK_CURRENT);
            }
        }
    }
}

void CGLRenderer::Update (PointLight& pointLight)
{
    unsigned int objectHandle = pointLight.GetHandle ();
    unsigned int lightId;
    unsigned int uniformBufferOffset;

    // check if light is already in collection
    auto find = m_PointLights.find (objectHandle);

    if (find == m_PointLights.end ())
    {
        lightId = m_UniformPointLightBuffer->pointLightCount++;
        m_PointLights.insert ({ objectHandle, lightId });
    }
    else
    {
        // existing light modified
        lightId = m_PointLights[objectHandle];
    }

    // copy position
    Vector4f lightPosition = pointLight.GetPosition ();
    m_UniformPointLightBuffer->pointLights[lightId].lightPosition[0] = lightPosition[0];
    m_UniformPointLightBuffer->pointLights[lightId].lightPosition[1] = lightPosition[1];
    m_UniformPointLightBuffer->pointLights[lightId].lightPosition[2] = lightPosition[2];

    // copy attenuation factors
    m_UniformPointLightBuffer->pointLights[lightId].attenuationConst = pointLight.GetConstantAttenuationFactor ();
    m_UniformPointLightBuffer->pointLights[lightId].attenuationLinear = pointLight.GetLinearAttenuationFactor ();
    m_UniformPointLightBuffer->pointLights[lightId].attenuationQuadratic = pointLight.GetQuadraticAttenuationFactor ();

    // copy color
    m_UniformPointLightBuffer->pointLights[lightId].lightColor[0] = pointLight.GetColor ()[0];
    m_UniformPointLightBuffer->pointLights[lightId].lightColor[1] = pointLight.GetColor ()[1];
    m_UniformPointLightBuffer->pointLights[lightId].lightColor[2] = pointLight.GetColor ()[2];

    // copy to GPU memory
    glBindBuffer (GL_UNIFORM_BUFFER, m_UniformBuffers->UBO[UBO_POINTLIGHTS]);

    // load light counts
    glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (m_UniformPointLightBuffer->pointLightCount), &m_UniformPointLightBuffer->pointLightCount);

    // load uniform buffer for a light at given index
    uniformBufferOffset = sizeof (m_UniformPointLightBuffer->pointLightCount) + 3 * sizeof (GLint) + lightId * sizeof (SGlPointLightStruct);
    glBufferSubData (GL_UNIFORM_BUFFER, uniformBufferOffset, sizeof (SGlPointLightStruct), &m_UniformPointLightBuffer->pointLights[lightId]);

    glBindBuffer (GL_UNIFORM_BUFFER, 0);

}

void CGLRenderer::Update (DirectionalLight& directionalLight)
{
    unsigned int objectHandle = directionalLight.GetHandle ();
    unsigned int lightId;
    unsigned int uniformBufferOffset;

    // check if light is already in collection
    auto find = m_DirectionalLights.find (objectHandle);

    if (find == m_DirectionalLights.end ())
    {
        lightId = m_UniformDirectionalLightBuffer->directionalLightCount++;
        m_DirectionalLights.insert ({ objectHandle, lightId });
    }
    else
    {
        // existing light modified
        lightId = m_DirectionalLights[objectHandle];
    }

    // copy direction
    Vector3f lightDirection = directionalLight.GetForward ();
    m_UniformDirectionalLightBuffer->directionalLights[lightId].lightDirection[0] = lightDirection[0];
    m_UniformDirectionalLightBuffer->directionalLights[lightId].lightDirection[1] = lightDirection[1];
    m_UniformDirectionalLightBuffer->directionalLights[lightId].lightDirection[2] = lightDirection[2];

    // copy color
    m_UniformDirectionalLightBuffer->directionalLights[lightId].lightColor[0] = directionalLight.GetColor ()[0];
    m_UniformDirectionalLightBuffer->directionalLights[lightId].lightColor[1] = directionalLight.GetColor ()[1];
    m_UniformDirectionalLightBuffer->directionalLights[lightId].lightColor[2] = directionalLight.GetColor ()[2];

    // copy to GPU memory
    glBindBuffer (GL_UNIFORM_BUFFER, m_UniformBuffers->UBO[UBO_DIRECTIONALLIGHTS]);

    // load light counts
    glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (m_UniformDirectionalLightBuffer->directionalLightCount), &m_UniformDirectionalLightBuffer->directionalLightCount);

    // load uniform buffer for a light at given index
    uniformBufferOffset = sizeof (m_UniformDirectionalLightBuffer->directionalLightCount) + 3 * sizeof (GLint) + lightId * sizeof (SGlDirectionalLightStruct);
    glBufferSubData (GL_UNIFORM_BUFFER, uniformBufferOffset, sizeof (SGlDirectionalLightStruct), &m_UniformDirectionalLightBuffer->directionalLights[lightId]);

    glBindBuffer (GL_UNIFORM_BUFFER, 0);
}

void CGLRenderer::Update (SpotLight& spotLight)
{
    unsigned int objectHandle = spotLight.GetHandle ();
    unsigned int lightId;
    unsigned int uniformBufferOffset;

    // check if light is already in collection
    auto find = m_SpotLights.find (objectHandle);

    if (find == m_SpotLights.end ())
    {
        lightId = m_UniformSpotLightBuffer->spotLightCount++;
        m_SpotLights.insert ({ objectHandle, lightId });
    }
    else
    {
        // existing light modified
        lightId = m_SpotLights[objectHandle];
    }

    // copy position
    Vector4f lightPosition = spotLight.GetPosition ();
    m_UniformSpotLightBuffer->spotLights[lightId].lightPosition[0] = lightPosition[0];
    m_UniformSpotLightBuffer->spotLights[lightId].lightPosition[1] = lightPosition[1];
    m_UniformSpotLightBuffer->spotLights[lightId].lightPosition[2] = lightPosition[2];

    // copy direction
    Vector3f lightDirection = spotLight.GetForward ();
    m_UniformSpotLightBuffer->spotLights[lightId].lightDirection[0] = lightDirection[0];
    m_UniformSpotLightBuffer->spotLights[lightId].lightDirection[1] = lightDirection[1];
    m_UniformSpotLightBuffer->spotLights[lightId].lightDirection[2] = lightDirection[2];
    
    // copy attenuation factors
    m_UniformSpotLightBuffer->spotLights[lightId].attenuationConst = spotLight.GetConstantAttenuationFactor ();
    m_UniformSpotLightBuffer->spotLights[lightId].attenuationLinear = spotLight.GetLinearAttenuationFactor ();
    m_UniformSpotLightBuffer->spotLights[lightId].attenuationQuadratic = spotLight.GetQuadraticAttenuationFactor ();

    // copy cutoff angles
    m_UniformSpotLightBuffer->spotLights[lightId].innerCutoffCosine = std::cos (Mathf::Deg2Rad (spotLight.GetInnerCutoff ()));
    m_UniformSpotLightBuffer->spotLights[lightId].outerCutoffCosine = std::cos (Mathf::Deg2Rad (spotLight.GetOuterCutoff ()));

    // copy color
    m_UniformSpotLightBuffer->spotLights[lightId].lightColor[0] = spotLight.GetColor ()[0];
    m_UniformSpotLightBuffer->spotLights[lightId].lightColor[1] = spotLight.GetColor ()[1];
    m_UniformSpotLightBuffer->spotLights[lightId].lightColor[2] = spotLight.GetColor ()[2];

    // copy to GPU memory
    glBindBuffer (GL_UNIFORM_BUFFER, m_UniformBuffers->UBO[UBO_SPOTLIGHTS]);

    // load light counts
    glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (m_UniformSpotLightBuffer->spotLightCount), &m_UniformSpotLightBuffer->spotLightCount);

    // load uniform buffer for a light at given index
    uniformBufferOffset = sizeof (m_UniformSpotLightBuffer->spotLightCount) + 3 * sizeof (GLint) + lightId * sizeof (SGlSpotLightStruct);
    glBufferSubData (GL_UNIFORM_BUFFER, uniformBufferOffset, sizeof (SGlSpotLightStruct), &m_UniformSpotLightBuffer->spotLights[lightId]);

    glBindBuffer (GL_UNIFORM_BUFFER, 0);
}

void CGLRenderer::UpdateCameraBuffers (Camera& camera)
{
    LoadMatrixUniformBuffers (&camera);
}

IFramebuffer* CGLRenderer::CreateFramebuffer (unsigned int rgbTextures, unsigned int rgbaTextures, bool multisampleEnabled)
{
    IFramebuffer* framebuffer;

    if (multisampleEnabled)
    {
#if (CILANTRO_GL_VERSION <= 140)
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "OpenGL 3.2 required for multisample framebuffers";
#else
        framebuffer = new CGLMultisampleFramebuffer (this->m_width, this->m_height, rgbTextures, rgbaTextures);
#endif
    }
    else
    {
        framebuffer = new CGLFramebuffer (this->m_width, this->m_height, rgbTextures, rgbaTextures);
    }
    
    framebuffer->Initialize ();

    return framebuffer;
}

void CGLRenderer::BindDefaultFramebuffer ()
{
    glBindFramebuffer (GL_FRAMEBUFFER, (GLint) 0);
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
        glFrontFace (GL_CCW);
        glEnable (GL_CULL_FACE);
    }
    else
    {   
        glDisable (GL_CULL_FACE);
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
    GLShaderProgram* p;

    // load standard shaders
    CGame::GetResourceManager ().Load<GLShader> ("default_vertex_shader", "shaders/default.vs", ShaderType::VERTEX_SHADER);
    CGame::GetResourceManager ().Load<GLShader> ("flatquad_vertex_shader", "shaders/flatquad.vs", ShaderType::VERTEX_SHADER);
    CGame::GetResourceManager ().Load<GLShader> ("pbr_forward_fragment_shader", "shaders/pbr_forward.fs", ShaderType::FRAGMENT_SHADER);    
    CGame::GetResourceManager ().Load<GLShader> ("pbr_deferred_geometrypass_fragment_shader", "shaders/pbr_deferred_geometrypass.fs", ShaderType::FRAGMENT_SHADER); 
    CGame::GetResourceManager ().Load<GLShader> ("pbr_deferred_lightingpass_fragment_shader", "shaders/pbr_deferred_lightingpass.fs", ShaderType::FRAGMENT_SHADER); 
    CGame::GetResourceManager ().Load<GLShader> ("blinnphong_forward_fragment_shader", "shaders/blinnphong_forward.fs", ShaderType::FRAGMENT_SHADER);
    CGame::GetResourceManager ().Load<GLShader> ("blinnphong_deferred_geometrypass_fragment_shader", "shaders/blinnphong_deferred_geometrypass.fs", ShaderType::FRAGMENT_SHADER); 
    CGame::GetResourceManager ().Load<GLShader> ("blinnphong_deferred_lightingpass_fragment_shader", "shaders/blinnphong_deferred_lightingpass.fs", ShaderType::FRAGMENT_SHADER); 
    CGame::GetResourceManager ().Load<GLShader> ("flatquad_fragment_shader", "shaders/flatquad.fs", ShaderType::FRAGMENT_SHADER);
    CGame::GetResourceManager ().Load<GLShader> ("post_hdr_fragment_shader", "shaders/post_hdr.fs", ShaderType::FRAGMENT_SHADER);
    CGame::GetResourceManager ().Load<GLShader> ("post_gamma_fragment_shader", "shaders/post_gamma.fs", ShaderType::FRAGMENT_SHADER);
    CGame::GetResourceManager ().Load<GLShader> ("post_fxaa_fragment_shader", "shaders/post_fxaa.fs", ShaderType::FRAGMENT_SHADER);

    // PBR model (forward)
    p = &AddShaderProgram<GLShaderProgram> ("pbr_forward_shader");
    p->AddShader ("default_vertex_shader");
    p->AddShader ("pbr_forward_fragment_shader");
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
#endif
    p->BindUniformBlock ("UniformMatricesBlock", BindingPoint::BP_MATRICES);
    p->BindUniformBlock ("UniformPointLightsBlock", BindingPoint::BP_POINTLIGHTS);
    p->BindUniformBlock ("UniformDirectionalLightsBlock", BindingPoint::BP_DIRECTIONALLIGHTS);
    p->BindUniformBlock ("UniformSpotLightsBlock", BindingPoint::BP_SPOTLIGHTS);

    // PBR model (deferred, geometry pass)
    p = &AddShaderProgram<GLShaderProgram> ("pbr_deferred_geometrypass_shader");
    p->AddShader ("default_vertex_shader");
    p->AddShader ("pbr_deferred_geometrypass_fragment_shader");
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
    p->BindUniformBlock ("UniformMatricesBlock", BindingPoint::BP_MATRICES);

    // PBR model (deferred, lighting pass)
    p = &AddShaderProgram<GLShaderProgram> ("pbr_deferred_lightingpass_shader");
    p->AddShader ("flatquad_vertex_shader");
    p->AddShader ("pbr_deferred_lightingpass_fragment_shader");
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
#endif
    p->BindUniformBlock ("UniformPointLightsBlock", BindingPoint::BP_POINTLIGHTS);
    p->BindUniformBlock ("UniformDirectionalLightsBlock", BindingPoint::BP_DIRECTIONALLIGHTS);
    p->BindUniformBlock ("UniformSpotLightsBlock", BindingPoint::BP_SPOTLIGHTS);

    // Blinn-Phong model (forward)
    p = &AddShaderProgram<GLShaderProgram> ("blinnphong_forward_shader");
    p->AddShader ("default_vertex_shader");
    p->AddShader ("blinnphong_forward_fragment_shader");
    p->Link ();
    p->Use ();
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation(p->GetProgramId ()0, "vPosition");
    glBindAttribLocation(p->GetProgramId ()1, "vNormal");
    glBindAttribLocation(p->GetProgramId ()2, "vUV");
    glBindAttribLocation(p->GetProgramId ()3, "vTangent");
    glBindAttribLocation(p->GetProgramId ()4, "vBitangent");
#endif
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tDiffuse"), 0);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tNormal"), 1);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tSpecular"), 2);
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "tEmissive"), 3);
#endif
    p->BindUniformBlock ("UniformMatricesBlock", BindingPoint::BP_MATRICES);
    p->BindUniformBlock ("UniformPointLightsBlock", BindingPoint::BP_POINTLIGHTS);
    p->BindUniformBlock ("UniformDirectionalLightsBlock", BindingPoint::BP_DIRECTIONALLIGHTS);
    p->BindUniformBlock ("UniformSpotLightsBlock", BindingPoint::BP_SPOTLIGHTS); 
    
    // Blinn-Phong model (deferred, geometry pass)
    p = &AddShaderProgram<GLShaderProgram> ("blinnphong_deferred_geometrypass_shader");
    p->AddShader ("default_vertex_shader");
    p->AddShader ("blinnphong_deferred_geometrypass_fragment_shader");
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
    p->BindUniformBlock ("UniformMatricesBlock", BindingPoint::BP_MATRICES);

    // Blinn-Phong model (deferred, lighting pass)
    p = &AddShaderProgram<GLShaderProgram> ("blinnphong_deferred_lightingpass_shader");
    p->AddShader ("flatquad_vertex_shader");
    p->AddShader ("blinnphong_deferred_lightingpass_fragment_shader");
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
#endif
    p->BindUniformBlock ("UniformPointLightsBlock", BindingPoint::BP_POINTLIGHTS);
    p->BindUniformBlock ("UniformDirectionalLightsBlock", BindingPoint::BP_DIRECTIONALLIGHTS);
    p->BindUniformBlock ("UniformSpotLightsBlock", BindingPoint::BP_SPOTLIGHTS);    
 
    // Screen quad rendering
    p = &AddShaderProgram<GLShaderProgram> ("flatquad_shader");
    p->AddShader ("flatquad_vertex_shader");
    p->AddShader ("flatquad_fragment_shader");   
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation (p->GetProgramId (), 0, "vPosition");
    glBindAttribLocation (p->GetProgramId (), 1, "vTextureCoordinates");
#endif
    p->Link ();
    p->Use (); 
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "fScreenTexture"), 0);
#endif

    // Post-processing HDR
    p = &AddShaderProgram<GLShaderProgram> ("post_hdr_shader");
    p->AddShader ("flatquad_vertex_shader");
    p->AddShader ("post_hdr_fragment_shader");
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation (p->GetProgramId (), 0, "vPosition");
    glBindAttribLocation (p->GetProgramId (), 1, "vTextureCoordinates");
#endif
    p->Link ();
    p->Use ();
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "fScreenTexture"), 0);
#endif

    // Post-processing gamma
    p = &AddShaderProgram<GLShaderProgram> ("post_gamma_shader");
    p->AddShader ("flatquad_vertex_shader");
    p->AddShader ("post_gamma_fragment_shader");   
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation (p->GetProgramId (), 0, "vPosition");
    glBindAttribLocation (p->GetProgramId (), 1, "vTextureCoordinates");
#endif
    p->Link ();
    p->Use (); 
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p->GetProgramId (), "fScreenTexture"), 0);
#endif

    // Post-processing fxaa
    p = &AddShaderProgram<GLShaderProgram> ("post_fxaa_shader");
    p->AddShader ("flatquad_vertex_shader");
    p->AddShader ("post_fxaa_fragment_shader");   
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

}

void CGLRenderer::InitializeMatrixUniformBuffers ()
{
    // create and pre-load uniform buffer for view and projection matrices
    glGenBuffers (1, &m_UniformBuffers->UBO[UBO_MATRICES]);
    glBindBuffer (GL_UNIFORM_BUFFER, m_UniformBuffers->UBO[UBO_MATRICES]);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGlUniformMatrixBuffer), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, BindingPoint::BP_MATRICES, m_UniformBuffers->UBO[UBO_MATRICES]);

    CheckGLError (MSG_LOCATION);
}

void CGLRenderer::LoadMatrixUniformBuffers (Camera* camera)
{
    SGlUniformBuffers* buffers = static_cast<SGlUniformBuffers*>(m_UniformBuffers);
    SGlUniformMatrixBuffer* mBuffer = static_cast<SGlUniformMatrixBuffer*>(m_UniformMatrixBuffer);

    // load view matrix
    std::memcpy (mBuffer->viewMatrix, Mathf::Transpose (camera->GetViewMatrix ())[0], 16 * sizeof (GLfloat));

    // load projection matrix
    std::memcpy (mBuffer->projectionMatrix, Mathf::Transpose (camera->GetProjectionMatrix (m_width, m_height))[0], 16 * sizeof (GLfloat));

    // load to GPU
    glBindBuffer (GL_UNIFORM_BUFFER, buffers->UBO[UBO_MATRICES]);
    glBufferSubData (GL_UNIFORM_BUFFER, 0, 16 * sizeof (GLfloat), mBuffer->viewMatrix);
    glBufferSubData (GL_UNIFORM_BUFFER, 16 * sizeof (GLfloat), 16 * sizeof (GLfloat), mBuffer->projectionMatrix);
    glBindBuffer (GL_UNIFORM_BUFFER, 0);
}

void CGLRenderer::DeinitializeMatrixUniformBuffers ()
{
    glDeleteBuffers (1, &m_UniformBuffers->UBO[UBO_MATRICES]);
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
    for (auto&& buffer : m_SceneGeometryBuffers)
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
        -1.0f,  1.0f, 
        -1.0f, -1.0f,
         1.0f, -1.0f,

        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f
    };

    float quadUV[] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f
    };

    glGenVertexArrays (1, &m_QuadGeometryBuffer->VAO);    
    glBindVertexArray (m_QuadGeometryBuffer->VAO);

    glGenBuffers (1, &m_QuadGeometryBuffer->VBO[EGlVboType::VBO_VERTICES]);
    glGenBuffers (1, &m_QuadGeometryBuffer->VBO[EGlVboType::VBO_UVS]);

    glBindBuffer (GL_ARRAY_BUFFER, m_QuadGeometryBuffer->VBO[EGlVboType::VBO_VERTICES]);
    glBufferData (GL_ARRAY_BUFFER, sizeof (quadVertices), &quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer (EGlVboType::VBO_VERTICES, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer (GL_ARRAY_BUFFER, m_QuadGeometryBuffer->VBO[EGlVboType::VBO_UVS]);
    glBufferData (GL_ARRAY_BUFFER, sizeof (quadUV), &quadUV, GL_STATIC_DRAW);
    glVertexAttribPointer (EGlVboType::VBO_UVS, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glEnableVertexAttribArray (EGlVboType::VBO_VERTICES);
    glEnableVertexAttribArray (EGlVboType::VBO_UVS);

    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindVertexArray (0);    

    m_QuadGeometryBuffer->vertexCount = 6;

    CheckGLError (MSG_LOCATION);
}

void CGLRenderer::DeinitializeQuadGeometryBuffer ()
{
    glDeleteVertexArrays(1, &m_QuadGeometryBuffer->VAO);
    glDeleteBuffers(1, &m_QuadGeometryBuffer->VBO[EGlVboType::VBO_VERTICES]);
}

void CGLRenderer::InitializeLightUniformBuffers ()
{
    m_UniformPointLightBuffer->pointLightCount = 0;
    m_UniformSpotLightBuffer->spotLightCount = 0;
    m_UniformDirectionalLightBuffer->directionalLightCount = 0;

    // create uniform buffer for point lights
    glGenBuffers (1, &m_UniformBuffers->UBO[UBO_POINTLIGHTS]);
    glBindBuffer (GL_UNIFORM_BUFFER, m_UniformBuffers->UBO[UBO_POINTLIGHTS]);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGlUniformPointLightBuffer), m_UniformPointLightBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, BindingPoint::BP_POINTLIGHTS, m_UniformBuffers->UBO[UBO_POINTLIGHTS]);

    // create uniform buffer for directional lights
    glGenBuffers (1, &m_UniformBuffers->UBO[UBO_DIRECTIONALLIGHTS]);
    glBindBuffer (GL_UNIFORM_BUFFER, m_UniformBuffers->UBO[UBO_DIRECTIONALLIGHTS]);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGlUniformDirectionalLightBuffer), m_UniformDirectionalLightBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, BindingPoint::BP_DIRECTIONALLIGHTS, m_UniformBuffers->UBO[UBO_DIRECTIONALLIGHTS]);

    // create uniform buffer for spot lights
    glGenBuffers (1, &m_UniformBuffers->UBO[UBO_SPOTLIGHTS]);
    glBindBuffer (GL_UNIFORM_BUFFER, m_UniformBuffers->UBO[UBO_SPOTLIGHTS]);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGlUniformSpotLightBuffer), m_UniformSpotLightBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, BindingPoint::BP_SPOTLIGHTS, m_UniformBuffers->UBO[UBO_SPOTLIGHTS]);

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
    glDeleteBuffers (1, &m_UniformBuffers->UBO[UBO_POINTLIGHTS]);
    glDeleteBuffers (1, &m_UniformBuffers->UBO[UBO_DIRECTIONALLIGHTS]);
    glDeleteBuffers (1, &m_UniformBuffers->UBO[UBO_SPOTLIGHTS]);
}

void CGLRenderer::UpdateLightBufferRecursive (unsigned int objectHandle)
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
    SGlGeometryBuffers* glBuffer = static_cast<SGlGeometryBuffers*>(buffer);

    // bind
    glBindVertexArray (glBuffer->VAO);    

    // draw
    glDrawArrays (GL_TRIANGLES, 0, glBuffer->vertexCount);

    // unbind
    glBindTexture (GL_TEXTURE_2D, 0);
    glBindVertexArray (0);
    glBindFramebuffer (GL_FRAMEBUFFER, 0);
}

GLuint CGLRenderer::GetTextureFormat (unsigned int numTextures)
{
    switch (numTextures)
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
}

void CGLRenderer::CheckGLError (const std::string& location)
{
    GLuint errorCode;

    if ((errorCode = glGetError ()) != GL_NO_ERROR)
    {
        LogMessage (location, EXIT_FAILURE) << "glError:" << std::hex << std::showbase << errorCode;
    }
}
