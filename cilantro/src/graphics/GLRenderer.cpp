#include "graphics/GLRenderer.h"
#include "graphics/GLUtils.h"
#include "graphics/GLShader.h"
#include "graphics/GLShaderProgram.h"
#include "graphics/GLFramebuffer.h"
#include "graphics/GLMultisampleFramebuffer.h"
#include "graphics/SurfaceRenderStage.h"

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
#include <cmath>
#include <cstring>
#include <array>
#include <bit>

namespace cilantro {

GLRenderer::GLRenderer (std::shared_ptr<GameScene> gameScene, unsigned int width, unsigned int height, bool shadowMappingEnabled, bool deferredRenderingEnabled) 
    : Renderer (gameScene, width, height, shadowMappingEnabled, deferredRenderingEnabled)
{
    m_surfaceGeometryBuffer = new SGlGeometryBuffers ();
    m_uniformBuffers = new SGlUniformBuffers ();
    m_uniformMatrixBuffer = new SGlUniformMatrixBuffer ();
    m_uniformLightViewMatrixBuffer = new SGlUniformLightViewMatrixBuffer ();
    m_uniformPointLightBuffer = new SGlUniformPointLightBuffer ();
    m_uniformDirectionalLightBuffer = new SGlUniformDirectionalLightBuffer ();
    m_uniformSpotLightBuffer = new SGlUniformSpotLightBuffer ();
}

GLRenderer::~GLRenderer ()
{
    for (auto&& objectBuffer : m_sceneGeometryBuffers)
    {
        delete objectBuffer.second;
    }

    delete m_surfaceGeometryBuffer;
    delete m_uniformBuffers;
    delete m_uniformMatrixBuffer;
    delete m_uniformLightViewMatrixBuffer;
    delete m_uniformPointLightBuffer;
    delete m_uniformDirectionalLightBuffer;
    delete m_uniformSpotLightBuffer;
}

void GLRenderer::Initialize ()
{    
    Renderer::Initialize ();

    GLUtils::PrintGLInfo ();
    GLUtils::PrintGLExtensions ();

    InitializeShaderLibrary ();
    InitializeQuadGeometryBuffer ();
    InitializeObjectBuffers ();
    InitializeMatrixUniformBuffers ();
    InitializeLightViewMatrixUniformBuffers ();
    InitializeLightUniformBuffers ();

    // set callback for new MeshObjects
    GetGameScene ()->GetGame ()->GetMessageBus ()->Subscribe<MeshObjectUpdateMessage> (
        [&](const std::shared_ptr<MeshObjectUpdateMessage>& message) 
        { 
            Update (GetGameScene ()->GetGameObjectManager ()->GetByHandle<MeshObject> (message->GetHandle ()));
            UpdateAABBBuffers (GetGameScene ()->GetGameObjectManager ()->GetByHandle<MeshObject> (message->GetHandle ()));
        }
    );

    // set callback for new or modified materials
    GetGameScene ()->GetGame ()->GetMessageBus ()->Subscribe<MaterialTextureUpdateMessage> (
        [&](const std::shared_ptr<MaterialTextureUpdateMessage>& message) 
        { 
            Update (GetGameScene ()->GetMaterialManager ()->GetByHandle<Material> (message->GetHandle ()), message->GetTextureUnit ());
        }
    );
    GetGameScene ()->GetGame ()->GetMessageBus ()->Subscribe<MaterialUpdateMessage> (
        [&](const std::shared_ptr<MaterialUpdateMessage>& message) 
        { 
            Update (GetGameScene ()->GetMaterialManager ()->GetByHandle<Material> (message->GetHandle ()));
        }
    );
    
    // set callback for new or modified lights
    GetGameScene ()->GetGame ()->GetMessageBus ()->Subscribe<LightUpdateMessage> (
        [&](const std::shared_ptr<LightUpdateMessage>& message) 
        { 
            GetGameScene ()->GetGameObjectManager ()->GetByHandle<GameObject> (message->GetHandle ())->OnUpdate (*this); 
        }
    );

    // set callback for modified scene graph (currently this only requires to reload light buffers)
    GetGameScene ()->GetGame ()->GetMessageBus ()->Subscribe<SceneGraphUpdateMessage> (
        [&](const std::shared_ptr<SceneGraphUpdateMessage>& message) 
        { 
            UpdateLightBufferRecursive (message->GetHandle ());
        }
    );

    // set callback for modified transforms (reload light buffers, reload AABB geometry buffers)
    GetGameScene ()->GetGame ()->GetMessageBus ()->Subscribe<TransformUpdateMessage> (
        [&](const std::shared_ptr<TransformUpdateMessage>& message) 
        { 
            m_invalidatedObjects.insert (message->GetHandle ());
        }
    );
    
}

void GLRenderer::Deinitialize ()
{
    Renderer::Deinitialize ();

    DeinitializeQuadGeometryBuffer ();
    DeinitializeObjectBuffers ();
    DeinitializeMatrixUniformBuffers ();
    DeinitializeLightViewMatrixUniformBuffers ();
    DeinitializeLightUniformBuffers ();
}

std::shared_ptr<IRenderer> GLRenderer::SetViewport (unsigned int x, unsigned int y, unsigned int sx, unsigned int sy)
{
    glViewport (x, y, sx, sy);

    return std::dynamic_pointer_cast<IRenderer> (shared_from_this ());
}

void GLRenderer::RenderFrame ()
{
    for (auto handle : m_invalidatedObjects)
    {
        // lights
        UpdateLightBufferRecursive (handle);

        // AABBs
        if (std::dynamic_pointer_cast<MeshObject> (GetGameScene ()->GetGameObjectManager ()->GetByHandle<GameObject> (handle)) != nullptr)
        {
            UpdateAABBBuffers (GetGameScene ()->GetGameObjectManager ()->GetByHandle<MeshObject> (handle));
        }
    }

    Renderer::RenderFrame ();
}

void GLRenderer::Draw (std::shared_ptr<MeshObject> meshObject)
{
    SGlGeometryBuffers* b = m_sceneGeometryBuffers[meshObject->GetHandle ()];
    GLuint shaderProgramId;

    auto objM = meshObject->GetMaterial ();

    // get shader program for rendered meshobject (geometry pass)
    auto geometryShaderProgram = m_shaderProgramManager->GetByName<GLShaderProgram> (
        m_isDeferredRendering
        ? objM->GetDeferredGeometryPassShaderProgram ()
        : objM->GetForwardShaderProgram ()
    );
    geometryShaderProgram->Use ();
    shaderProgramId = geometryShaderProgram->GetProgramId ();

    // bind textures for active material and bind a shadow map
    if (m_materialTextureUnits.find(meshObject->GetMaterial ()->GetHandle ()) != m_materialTextureUnits.end ())
    {
        SGlMaterialTextureUnits* u = m_materialTextureUnits[meshObject->GetMaterial ()->GetHandle ()];

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
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Missing texture for object" << meshObject->GetHandle ();
    }

    // set material uniforms for active material
    for (auto&& property : meshObject->GetMaterial ()->GetPropertiesMap ())
    {
        if (geometryShaderProgram->HasUniform (property.first.c_str ()))
        {
            if (property.second.size () == 1)
            {
                geometryShaderProgram->SetUniformFloat (property.first.c_str (), property.second[0]);
            }
            else if ((property.second.size () == 3))
            {
                geometryShaderProgram->SetUniformFloatv (property.first.c_str (), property.second.data (), 3);
            }
            else
            {
                LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Invalid vector size for material property" << property.first << "in shader" << geometryShaderProgram->GetName () << "for" << meshObject->GetName ();
            }
        }        
        else 
        {
            LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Invalid material uniform" << property.first << "in shader" << geometryShaderProgram->GetName () << "for" << meshObject->GetName ();
        }
    }

    // get world matrix for drawn objects and set uniform value
    geometryShaderProgram->SetUniformMatrix4f ("mModel", meshObject->GetWorldTransformMatrix ());

    // calculate normal matrix for drawn objects and set uniform value
    geometryShaderProgram->SetUniformMatrix3f ("mNormal", Mathf::Invert (Mathf::Transpose (Matrix3f (meshObject->GetWorldTransformMatrix ()))));

    // get camera position in world space and set uniform value
    if (!m_isDeferredRendering)
    {
        geometryShaderProgram->SetUniformVector3f ("eyePosition", GetGameScene ()->GetActiveCamera ()->GetPosition ());
    }

    // get shader program for rendered meshobject (lighting pass)
    if (m_isDeferredRendering)
    {
        auto lightingShaderProgram = m_shaderProgramManager->GetByName<GLShaderProgram>(objM->GetDeferredLightingPassShaderProgram ());
        lightingShaderProgram->Use ();
        shaderProgramId = lightingShaderProgram->GetProgramId ();

        // get camera position in world space and set uniform value (this needs to be done again for deferred lighting shader program)
        lightingShaderProgram->SetUniformVector3f ("eyePosition", GetGameScene ()->GetActiveCamera ()->GetPosition ());

    }

    // load bone transformation matrix array to buffer
    glBindBuffer (GL_UNIFORM_BUFFER, b->boneTransformationsUBO);
    glBufferData (GL_UNIFORM_BUFFER, CILANTRO_MAX_BONES * sizeof (GLfloat) * 16, meshObject->GetBoneTransformationsMatrixArray (true), GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, static_cast<int>(EGlUBOType::UBO_BONETRANSFORMATIONS), b->boneTransformationsUBO);

    // draw mesh
    geometryShaderProgram->Use ();
    RenderGeometryBuffer (b, GL_TRIANGLES);
}

void GLRenderer::DrawSurface ()
{
    RenderGeometryBuffer (m_surfaceGeometryBuffer, GL_TRIANGLES);
}

void GLRenderer::DrawSceneGeometryBuffers (std::shared_ptr<IShaderProgram> shader)
{
    shader->Use ();

    for (auto&& geometryBuffer : m_sceneGeometryBuffers)
    {
        auto m = GetGameScene ()->GetGameObjectManager ()->GetByHandle<MeshObject> (geometryBuffer.first);

        // load model matrix to currently bound shader
        shader->SetUniformMatrix4f ("mModel", m->GetWorldTransformMatrix ());

        // load bone transformation matrix array to buffer
        glBindBuffer (GL_UNIFORM_BUFFER, m_sceneGeometryBuffers[m->GetHandle ()]->boneTransformationsUBO);
        glBufferData (GL_UNIFORM_BUFFER, CILANTRO_MAX_BONES * sizeof (float) * 16, m->GetBoneTransformationsMatrixArray (true), GL_DYNAMIC_DRAW);
        glBindBufferBase (GL_UNIFORM_BUFFER, static_cast<int>(EGlUBOType::UBO_BONETRANSFORMATIONS), m_sceneGeometryBuffers[m->GetHandle ()]->boneTransformationsUBO);

        // draw
        RenderGeometryBuffer (geometryBuffer.second, GL_TRIANGLES);
    }
}

void GLRenderer::DrawAABBGeometryBuffers (std::shared_ptr<IShaderProgram> shader)
{
    shader->Use ();

    for (auto&& geometryBuffer : m_aabbGeometryBuffers)
    {
        RenderGeometryBuffer (geometryBuffer.second, GL_LINES);
    }
}

void GLRenderer::Update (std::shared_ptr<MeshObject> meshObject)
{
    handle_t objectHandle = meshObject->GetHandle ();

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
        glGenBuffers (1, &b->VBO[EGlVBOType::VBO_VERTICES]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVBOType::VBO_VERTICES]);
        // location = 0 (vertex position)
        glVertexAttribPointer (EGlVBOType::VBO_VERTICES, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate normals buffer
        glGenBuffers (1, &b->VBO[EGlVBOType::VBO_NORMALS]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVBOType::VBO_NORMALS]);
        // location = 1 (vertex normal)
        glVertexAttribPointer (EGlVBOType::VBO_NORMALS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate uv buffer
        glGenBuffers (1, &b->VBO[EGlVBOType::VBO_UVS]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVBOType::VBO_UVS]);
        // location = 2 (vertex uv)
        glVertexAttribPointer (EGlVBOType::VBO_UVS, 2, GL_FLOAT, GL_FALSE, 2 * sizeof (float), (GLvoid*)0);

        // generate tangents buffer
        glGenBuffers (1, &b->VBO[EGlVBOType::VBO_TANGENTS]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVBOType::VBO_TANGENTS]);
        // location = 3 (vertex tangent)
        glVertexAttribPointer (EGlVBOType::VBO_TANGENTS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate bitangents buffer
        glGenBuffers (1, &b->VBO[EGlVBOType::VBO_BITANGENTS]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVBOType::VBO_BITANGENTS]);
        // location = 4 (vertex bitangent)
        glVertexAttribPointer (EGlVBOType::VBO_BITANGENTS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate bone index
        glGenBuffers (1, &b->VBO[EGlVBOType::VBO_BONES]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVBOType::VBO_BONES]);
        // location = 5 (bone indices)
        glVertexAttribIPointer (EGlVBOType::VBO_BONES, CILANTRO_MAX_BONE_INFLUENCES, GL_INT, CILANTRO_MAX_BONE_INFLUENCES * sizeof (int), (GLvoid*)0);

        // generate bone weight buffer
        glGenBuffers (1, &b->VBO[EGlVBOType::VBO_BONEWEIGHTS]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVBOType::VBO_BONEWEIGHTS]);
        // location = 6 (bone weights)
        glVertexAttribPointer (EGlVBOType::VBO_BONEWEIGHTS, CILANTRO_MAX_BONE_INFLUENCES, GL_FLOAT, GL_FALSE, CILANTRO_MAX_BONE_INFLUENCES * sizeof (int), (GLvoid*)0);

        // generate bone transformation matrix array uniform buffer
        glGenBuffers (1, &b->boneTransformationsUBO);
        glBindBuffer (GL_UNIFORM_BUFFER, b->boneTransformationsUBO);
        glBufferData (GL_UNIFORM_BUFFER, CILANTRO_MAX_BONES * sizeof (GLfloat) * 16, NULL, GL_DYNAMIC_DRAW);
        glBindBufferBase (GL_UNIFORM_BUFFER, static_cast<int>(EGlUBOType::UBO_BONETRANSFORMATIONS), b->boneTransformationsUBO);


        if (GLUtils::GetGLSLVersion ().versionNumber >= 430)
        {
            // generate vertex positions array SSBO buffer
            glGenBuffers (1, &b->vertexPositionsSSBO);
            glBindBuffer (GL_SHADER_STORAGE_BUFFER, b->vertexPositionsSSBO);
            glBufferData (GL_SHADER_STORAGE_BUFFER, CILANTRO_MAX_VERTICES * sizeof (GLfloat) * 3, NULL, GL_DYNAMIC_DRAW);
            glBindBufferBase (GL_SHADER_STORAGE_BUFFER, static_cast<int>(EGlSSBOType::SSBO_VERTICES), b->vertexPositionsSSBO);

            // generate bone indices array SSBO buffer
            glGenBuffers (1, &b->boneIndicesSSBO);
            glBindBuffer (GL_SHADER_STORAGE_BUFFER, b->boneIndicesSSBO);
            glBufferData (GL_SHADER_STORAGE_BUFFER, CILANTRO_MAX_VERTICES * sizeof (GLuint) * CILANTRO_MAX_BONE_INFLUENCES, NULL, GL_DYNAMIC_DRAW);
            glBindBufferBase (GL_SHADER_STORAGE_BUFFER, static_cast<int>(EGlSSBOType::SSBO_BONEINDICES), b->boneIndicesSSBO);

            // generate bone weights array SSBO buffer
            glGenBuffers (1, &b->boneWeightsSSBO);
            glBindBuffer (GL_SHADER_STORAGE_BUFFER, b->boneWeightsSSBO);
            glBufferData (GL_SHADER_STORAGE_BUFFER, CILANTRO_MAX_VERTICES * sizeof (GLfloat) * CILANTRO_MAX_BONE_INFLUENCES, NULL, GL_DYNAMIC_DRAW);
            glBindBufferBase (GL_SHADER_STORAGE_BUFFER, static_cast<int>(EGlSSBOType::SSBO_BONEWEIGHTS), b->boneWeightsSSBO);

            // generate AABB result SSBO buffer
            glGenBuffers (1, &b->aabbSSBO);
            glBindBuffer (GL_SHADER_STORAGE_BUFFER, b->aabbSSBO);
            glBufferData (GL_SHADER_STORAGE_BUFFER, sizeof (SGlEncodedAABB), NULL, GL_DYNAMIC_DRAW);
            glBindBufferBase (GL_SHADER_STORAGE_BUFFER, static_cast<int>(EGlSSBOType::SSBO_AABB), b->aabbSSBO);
        }

        // generate index buffer
        glGenBuffers (1, &b->EBO);
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, b->EBO);

        // enable VBO arrays
        glEnableVertexAttribArray (EGlVBOType::VBO_VERTICES);
        glEnableVertexAttribArray (EGlVBOType::VBO_NORMALS);
        glEnableVertexAttribArray (EGlVBOType::VBO_UVS);
        glEnableVertexAttribArray (EGlVBOType::VBO_TANGENTS);
        glEnableVertexAttribArray (EGlVBOType::VBO_BITANGENTS);
        glEnableVertexAttribArray (EGlVBOType::VBO_BONES);
        glEnableVertexAttribArray (EGlVBOType::VBO_BONEWEIGHTS);

        // unbind VAO
        glBindVertexArray (0);

    }

    // resize buffers and load data
    SGlGeometryBuffers* b = m_sceneGeometryBuffers[objectHandle];
    b->indexCount = meshObject->GetMesh ()->GetIndexCount ();

    // bind Vertex Array Object (VAO)
    glBindVertexArray (b->VAO);

    // load vertex buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVBOType::VBO_VERTICES]);
    glBufferData (GL_ARRAY_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (float) * 3, meshObject->GetMesh ()->GetVerticesData (), GL_DYNAMIC_DRAW);

    // load normals buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVBOType::VBO_NORMALS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (float) * 3, meshObject->GetMesh ()->GetNormalsData (), GL_DYNAMIC_DRAW);
    
    // load uv buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVBOType::VBO_UVS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (float) * 2, meshObject->GetMesh ()->GetUVData (), GL_DYNAMIC_DRAW);

    // load tangents buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVBOType::VBO_TANGENTS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (float) * 3, meshObject->GetMesh ()->GetTangentData (), GL_DYNAMIC_DRAW);

    // load bitangents buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVBOType::VBO_BITANGENTS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (float) * 3, meshObject->GetMesh ()->GetBitangentData (), GL_DYNAMIC_DRAW);

    // load bone index buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVBOType::VBO_BONES]);
    glBufferData (GL_ARRAY_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (uint32_t) * CILANTRO_MAX_BONE_INFLUENCES, meshObject->GetMesh ()->GetBoneIndicesData (), GL_DYNAMIC_DRAW);

    // load bone weight buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EGlVBOType::VBO_BONEWEIGHTS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (float) * CILANTRO_MAX_BONE_INFLUENCES, meshObject->GetMesh ()->GetBoneWeightsData (), GL_DYNAMIC_DRAW);

    // load index buffer
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, b->EBO);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, meshObject->GetMesh ()->GetIndexCount () * sizeof (uint32_t), meshObject->GetMesh ()->GetFacesData (), GL_DYNAMIC_DRAW);

    // unbind VAO
    glBindVertexArray (0);

}

void GLRenderer::UpdateAABBBuffers (std::shared_ptr<MeshObject> meshObject)
{
    handle_t objectHandle = meshObject->GetHandle ();

    // check of object's buffers are already initialized
    auto find = m_aabbGeometryBuffers.find (objectHandle);

    if (find == m_aabbGeometryBuffers.end ())
    {
        // it is a new object, so generate buffers 
        SGlGeometryBuffers* w = new SGlGeometryBuffers ();
        m_aabbGeometryBuffers.insert ({ objectHandle, w });
        w->indexCount = 12; // AABB has 12 edges

        // generate and bind Vertex Array Object (VAO) - wireframes
        glGenVertexArrays (1, &w->VAO);
        glBindVertexArray (w->VAO);

        // generate vertex buffer - wireframes
        glGenBuffers (1, &w->VBO[EGlVBOType::VBO_VERTICES]);
        glBindBuffer (GL_ARRAY_BUFFER, w->VBO[EGlVBOType::VBO_VERTICES]);
        // location = 0 (vertex position)
        glVertexAttribPointer (EGlVBOType::VBO_VERTICES, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate index buffer
        glGenBuffers (1, &w->EBO);
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, w->EBO);

        // load index buffer - wireframes (this is static)
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, w->EBO);
        glBufferData (GL_ELEMENT_ARRAY_BUFFER, 12 * 2 * sizeof (uint32_t), meshObject->GetAABB ().GetLineIndicesData (), GL_STATIC_DRAW);

        // enable VBO arrays
        glEnableVertexAttribArray (EGlVBOType::VBO_VERTICES);

        // unbind VAO
        glBindVertexArray (0);

    }

    // reload data
    SGlGeometryBuffers* w = m_aabbGeometryBuffers[objectHandle];

    // bind Vertex Array Object (VAO) - wireframes
    glBindVertexArray (w->VAO);

    // load vertex buffer - wireframes
    glBindBuffer (GL_ARRAY_BUFFER, w->VBO[EGlVBOType::VBO_VERTICES]);
    glBufferData (GL_ARRAY_BUFFER, 8 * sizeof (float) * 3, meshObject->GetAABB ().GetVerticesData () , GL_DYNAMIC_DRAW);
    
    // unbind VAO - wireframes
    glBindVertexArray (0);

}

AABB GLRenderer::CalculateAABB (std::shared_ptr<MeshObject> meshObject)
{
    if (GLUtils::GetGLSLVersion ().versionNumber >= 430)
    {
        // calculate in GPU

        AABB aabb;
        SGlEncodedAABB aabbGPU;
        SGlGeometryBuffers* b = m_sceneGeometryBuffers[meshObject->GetHandle ()];

        // get compute shader
        auto computeShader = m_shaderProgramManager->GetByName<GLShaderProgram> ("aabb_compute_shader");
        computeShader->Use ();
        
        // get world matrix for drawn objects and set uniform value
        computeShader->SetUniformMatrix4f ("mModel", meshObject->GetWorldTransformMatrix ());

        // load bone transformation matrix array to buffer
        glBindBuffer (GL_UNIFORM_BUFFER, b->boneTransformationsUBO);
        glBufferData (GL_UNIFORM_BUFFER, CILANTRO_MAX_BONES * sizeof (GLfloat) * 16, meshObject->GetBoneTransformationsMatrixArray (true), GL_DYNAMIC_DRAW);
        glBindBufferBase (GL_UNIFORM_BUFFER, static_cast<int>(EGlUBOType::UBO_BONETRANSFORMATIONS), b->boneTransformationsUBO);

        // load vertex positions array buffer (SSBO)
        glBindBuffer (GL_SHADER_STORAGE_BUFFER, b->vertexPositionsSSBO);
        glBufferData (GL_SHADER_STORAGE_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (GLfloat) * 3, meshObject->GetMesh ()->GetVerticesData (), GL_DYNAMIC_DRAW);
        glBindBufferBase (GL_SHADER_STORAGE_BUFFER, static_cast<int>(EGlSSBOType::SSBO_VERTICES), b->vertexPositionsSSBO);

        // load bone indices array buffer (SSBO)
        glBindBuffer (GL_SHADER_STORAGE_BUFFER, b->boneIndicesSSBO);
        glBufferData (GL_SHADER_STORAGE_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (GLuint) * CILANTRO_MAX_BONE_INFLUENCES, meshObject->GetMesh ()->GetBoneIndicesData (), GL_DYNAMIC_DRAW);
        glBindBufferBase (GL_SHADER_STORAGE_BUFFER, static_cast<int>(EGlSSBOType::SSBO_BONEINDICES), b->boneIndicesSSBO);

        // load bone weights array buffer (SSBO)
        glBindBuffer (GL_SHADER_STORAGE_BUFFER, b->boneWeightsSSBO);
        glBufferData (GL_SHADER_STORAGE_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (GLfloat) * CILANTRO_MAX_BONE_INFLUENCES, meshObject->GetMesh ()->GetBoneWeightsData (), GL_DYNAMIC_DRAW);
        glBindBufferBase (GL_SHADER_STORAGE_BUFFER, static_cast<int>(EGlSSBOType::SSBO_BONEWEIGHTS), b->boneWeightsSSBO);

        // initialize AABB extreme values
        aabbGPU.minBits[0] = 0xFFFFFFFF;
        aabbGPU.minBits[1] = 0xFFFFFFFF;
        aabbGPU.minBits[2] = 0xFFFFFFFF;
        aabbGPU.pad1 = 0x00000000;
        aabbGPU.maxBits[0] = 0x00000000;
        aabbGPU.maxBits[1] = 0x00000000;
        aabbGPU.maxBits[2] = 0x00000000;
        aabbGPU.pad2 = 0x00000000;
        glBindBuffer (GL_SHADER_STORAGE_BUFFER, b->aabbSSBO);
        glBufferData (GL_SHADER_STORAGE_BUFFER, sizeof (SGlEncodedAABB), &aabbGPU, GL_DYNAMIC_DRAW);
        glBindBufferBase (GL_SHADER_STORAGE_BUFFER, static_cast<int>(EGlSSBOType::SSBO_AABB), b->aabbSSBO);

        // dispatch compute shader
        GLuint groupSize = (static_cast<GLuint>(meshObject->GetMesh ()->GetVertexCount ()) + CILANTRO_COMPUTE_GROUP_SIZE - 1) / CILANTRO_COMPUTE_GROUP_SIZE;
        computeShader->Compute (groupSize, 1, 1);

        // read back AABB from compute shader
        glBindBuffer (GL_SHADER_STORAGE_BUFFER, b->aabbSSBO);
        glGetBufferSubData (GL_SHADER_STORAGE_BUFFER, 0, sizeof (SGlEncodedAABB), &aabbGPU);
        
        // redo the bit flip for the float representation
        auto toFloat = [](std::uint32_t u) -> float {
            std::uint32_t bits = (u & 0x80000000u)
                ? (u & 0x7FFFFFFFu)
                : ~u;
            return std::bit_cast<float>(bits);
        };

        // create the AABB object
        aabb.AddVertex (Vector3f (toFloat (aabbGPU.minBits[0]), toFloat (aabbGPU.minBits[1]), toFloat (aabbGPU.minBits[2])));
        aabb.AddVertex (Vector3f (toFloat (aabbGPU.maxBits[0]), toFloat (aabbGPU.maxBits[1]), toFloat (aabbGPU.maxBits[2])));

        return aabb;

    }
    else
    {
        // fall back to CPU calculation
        return Renderer::CalculateAABB (meshObject);
    }
}

void GLRenderer::Update (std::shared_ptr<Material> material, unsigned int textureUnit)
{
    handle_t materialHandle = material->GetHandle ();
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

    texture_map_t& textures = material->GetTexturesMap ();

    // check if material already exists
    auto find = m_materialTextureUnits.find (materialHandle);

    if (find == m_materialTextureUnits.end ())
    {
        m_materialTextureUnits.insert ({ materialHandle, new SGlMaterialTextureUnits () });
        
        for (auto&& t : textures)
        {
            auto tPtr = t.second.second;
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
        auto tPtr = t.second;
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

void GLRenderer::Update (std::shared_ptr<Material> material)
{
    handle_t shaderProgramHandle = m_shaderProgramManager->GetByName<ShaderProgram>(material->GetDeferredLightingPassShaderProgram ())->GetHandle ();
    std::string shaderProgramName = material->GetDeferredLightingPassShaderProgram ();

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
            auto q = Create <SurfaceRenderStage> ("deferred_lighting_" + shaderProgramName);
            q->SetShaderProgram (shaderProgramName);
            q->SetStencilTestEnabled (true)->SetStencilTest (EStencilTestFunction::FUNCTION_EQUAL, static_cast<int> (shaderProgramHandle));
            q->SetClearColorOnFrameEnabled (true);
            q->SetClearDepthOnFrameEnabled (false);
            q->SetClearStencilOnFrameEnabled (false);
            q->SetDepthTestEnabled (false);
            q->SetColorAttachmentsFramebufferLink (m_isShadowMapping ? EPipelineLink::LINK_SECOND : EPipelineLink::LINK_FIRST);
            q->SetDepthStencilFramebufferLink (m_isShadowMapping ? EPipelineLink::LINK_SECOND : EPipelineLink::LINK_FIRST);
            q->SetDepthArrayFramebufferLink (m_isShadowMapping ? EPipelineLink::LINK_FIRST : EPipelineLink::LINK_CURRENT);
            q->SetDrawFramebufferLink (m_isShadowMapping ? EPipelineLink::LINK_THIRD : EPipelineLink::LINK_SECOND);
            q->SetFramebufferEnabled (true);

            q->Initialize ();

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

                auto stage = m_renderStageManager->GetByHandle<SurfaceRenderStage> (stageHandle);
                stage->SetClearColorOnFrameEnabled (false);
                stage->SetFramebufferEnabled (false);
            }

        }
    }
}

void GLRenderer::Update (std::shared_ptr<PointLight> pointLight)
{
    handle_t objectHandle = pointLight->GetHandle ();
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
    Vector4f lightPosition = pointLight->GetPosition ();
    m_uniformPointLightBuffer->pointLights[lightId].lightPosition[0] = lightPosition[0];
    m_uniformPointLightBuffer->pointLights[lightId].lightPosition[1] = lightPosition[1];
    m_uniformPointLightBuffer->pointLights[lightId].lightPosition[2] = lightPosition[2];

    // copy attenuation factors
    m_uniformPointLightBuffer->pointLights[lightId].attenuationConst = pointLight->GetConstantAttenuationFactor ();
    m_uniformPointLightBuffer->pointLights[lightId].attenuationLinear = pointLight->GetLinearAttenuationFactor ();
    m_uniformPointLightBuffer->pointLights[lightId].attenuationQuadratic = pointLight->GetQuadraticAttenuationFactor ();

    // copy color
    m_uniformPointLightBuffer->pointLights[lightId].lightColor[0] = pointLight->GetColor ()[0];
    m_uniformPointLightBuffer->pointLights[lightId].lightColor[1] = pointLight->GetColor ()[1];
    m_uniformPointLightBuffer->pointLights[lightId].lightColor[2] = pointLight->GetColor ()[2];

    // copy to GPU memory
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_POINTLIGHTS]);

    // load light counts
    glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (m_uniformPointLightBuffer->pointLightCount), &m_uniformPointLightBuffer->pointLightCount);

    // load uniform buffer for a light at given index
    uniformBufferOffset = sizeof (m_uniformPointLightBuffer->pointLightCount) + 3 * sizeof (GLint) + lightId * sizeof (SGlPointLightStruct);
    glBufferSubData (GL_UNIFORM_BUFFER, uniformBufferOffset, sizeof (SGlPointLightStruct), &m_uniformPointLightBuffer->pointLights[lightId]);

    glBindBuffer (GL_UNIFORM_BUFFER, 0);

}

void GLRenderer::Update (std::shared_ptr<DirectionalLight> directionalLight)
{
    handle_t objectHandle = directionalLight->GetHandle ();
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
    auto shadowmapShader = GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader> ("shadowmap_directional_geometry_shader");
    shadowmapShader->SetParameter ("%%ACTIVE_DIRECTIONAL_LIGHTS%%", std::to_string (GetDirectionalLightCount ()));
    shadowmapShader->Compile ();

    auto shadowmapShaderProg = GetShaderProgramManager ()->GetByName<GLShaderProgram> ("shadowmap_directional_shader");
    shadowmapShaderProg->Link ();
    shadowmapShaderProg->BindUniformBlock ("UniformDirectionalLightViewMatricesBlock", EGlUBOType::UBO_DIRECTIONALLIGHTVIEWMATRICES);
    shadowmapShaderProg->BindUniformBlock ("UniformBoneTransformationsBlock", EGlUBOType::UBO_BONETRANSFORMATIONS);

    // copy direction
    Vector3f lightDirection = directionalLight->GetForward ();
    m_uniformDirectionalLightBuffer->directionalLights[lightId].lightDirection[0] = lightDirection[0];
    m_uniformDirectionalLightBuffer->directionalLights[lightId].lightDirection[1] = lightDirection[1];
    m_uniformDirectionalLightBuffer->directionalLights[lightId].lightDirection[2] = lightDirection[2];

    // copy color
    m_uniformDirectionalLightBuffer->directionalLights[lightId].lightColor[0] = directionalLight->GetColor ()[0];
    m_uniformDirectionalLightBuffer->directionalLights[lightId].lightColor[1] = directionalLight->GetColor ()[1];
    m_uniformDirectionalLightBuffer->directionalLights[lightId].lightColor[2] = directionalLight->GetColor ()[2];

    // copy to GPU memory
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTS]);

    // load light counts
    glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (m_uniformDirectionalLightBuffer->directionalLightCount), &m_uniformDirectionalLightBuffer->directionalLightCount);

    // load uniform buffer for a light at given index
    uniformBufferOffset = sizeof (m_uniformDirectionalLightBuffer->directionalLightCount) + 3 * sizeof (GLint) + lightId * sizeof (SGlDirectionalLightStruct);
    glBufferSubData (GL_UNIFORM_BUFFER, uniformBufferOffset, sizeof (SGlDirectionalLightStruct), &m_uniformDirectionalLightBuffer->directionalLights[lightId]);

    glBindBuffer (GL_UNIFORM_BUFFER, 0);
}

void GLRenderer::Update (std::shared_ptr<SpotLight> spotLight)
{
    handle_t objectHandle = spotLight->GetHandle ();
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
    Vector4f lightPosition = spotLight->GetPosition ();
    m_uniformSpotLightBuffer->spotLights[lightId].lightPosition[0] = lightPosition[0];
    m_uniformSpotLightBuffer->spotLights[lightId].lightPosition[1] = lightPosition[1];
    m_uniformSpotLightBuffer->spotLights[lightId].lightPosition[2] = lightPosition[2];

    // copy direction
    Vector3f lightDirection = spotLight->GetForward ();
    m_uniformSpotLightBuffer->spotLights[lightId].lightDirection[0] = lightDirection[0];
    m_uniformSpotLightBuffer->spotLights[lightId].lightDirection[1] = lightDirection[1];
    m_uniformSpotLightBuffer->spotLights[lightId].lightDirection[2] = lightDirection[2];
    
    // copy attenuation factors
    m_uniformSpotLightBuffer->spotLights[lightId].attenuationConst = spotLight->GetConstantAttenuationFactor ();
    m_uniformSpotLightBuffer->spotLights[lightId].attenuationLinear = spotLight->GetLinearAttenuationFactor ();
    m_uniformSpotLightBuffer->spotLights[lightId].attenuationQuadratic = spotLight->GetQuadraticAttenuationFactor ();

    // copy cutoff angles
    m_uniformSpotLightBuffer->spotLights[lightId].innerCutoffCosine = std::cos (Mathf::Deg2Rad (spotLight->GetInnerCutoff ()));
    m_uniformSpotLightBuffer->spotLights[lightId].outerCutoffCosine = std::cos (Mathf::Deg2Rad (spotLight->GetOuterCutoff ()));

    // copy color
    m_uniformSpotLightBuffer->spotLights[lightId].lightColor[0] = spotLight->GetColor ()[0];
    m_uniformSpotLightBuffer->spotLights[lightId].lightColor[1] = spotLight->GetColor ()[1];
    m_uniformSpotLightBuffer->spotLights[lightId].lightColor[2] = spotLight->GetColor ()[2];

    // copy to GPU memory
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_SPOTLIGHTS]);

    // load light counts
    glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (m_uniformSpotLightBuffer->spotLightCount), &m_uniformSpotLightBuffer->spotLightCount);

    // load uniform buffer for a light at given index
    uniformBufferOffset = sizeof (m_uniformSpotLightBuffer->spotLightCount) + 3 * sizeof (GLint) + lightId * sizeof (SGlSpotLightStruct);
    glBufferSubData (GL_UNIFORM_BUFFER, uniformBufferOffset, sizeof (SGlSpotLightStruct), &m_uniformSpotLightBuffer->spotLights[lightId]);

    glBindBuffer (GL_UNIFORM_BUFFER, 0);
}

void GLRenderer::UpdateCameraBuffers (std::shared_ptr<Camera> camera)
{
    LoadMatrixUniformBuffers (camera);
}

void GLRenderer::UpdateLightViewBuffers ()
{
    LoadLightViewMatrixUniformBuffers ();
}

size_t GLRenderer::GetPointLightCount () const
{
    return m_uniformPointLightBuffer->pointLightCount;
}

size_t GLRenderer::GetDirectionalLightCount () const
{
    return m_uniformDirectionalLightBuffer->directionalLightCount;
}

size_t GLRenderer::GetSpotLightCount () const
{
    return m_uniformSpotLightBuffer->spotLightCount;
}

std::shared_ptr<IFramebuffer> GLRenderer::CreateFramebuffer (unsigned int width, unsigned int height, unsigned int rgbTextureCount, unsigned int rgbaTextureCount, unsigned int depthBufferArrayTextureCount, bool depthStencilRenderbufferEnabled, bool multisampleEnabled)
{
    std::shared_ptr<IFramebuffer> framebuffer;

    if (multisampleEnabled)
    {
        if (GLUtils::GetGLSLVersion ().versionNumber <= 150)
        {
            LogMessage (MSG_LOCATION, EXIT_FAILURE) << "OpenGL 3.2 required for multisample framebuffers";
        }
        else 
        {
            framebuffer = std::make_shared<GLMultisampleFramebuffer> (width, height, rgbTextureCount, rgbaTextureCount, depthBufferArrayTextureCount, depthStencilRenderbufferEnabled);
        }
    }
    else
    {
        framebuffer = std::make_shared<GLFramebuffer> (width, height, rgbTextureCount, rgbaTextureCount, depthBufferArrayTextureCount, depthStencilRenderbufferEnabled);
    }
    
    framebuffer->Initialize ();

    return framebuffer;
}

void GLRenderer::BindDefaultFramebuffer ()
{
    glBindFramebuffer (GL_FRAMEBUFFER, (GLint) 0);
}

void GLRenderer::BindDefaultDepthBuffer ()
{
    glFramebufferTexture (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
}

void GLRenderer::BindDefaultStencilBuffer ()
{
    glFramebufferTexture (GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, 0, 0);
}

void GLRenderer::BindDefaultTextures ()
{
    for (unsigned int i = 0; i < CILANTRO_MAX_TEXTURE_UNITS; ++i)
    {
        glActiveTexture (GL_TEXTURE0 + i);
        glBindTexture (GL_TEXTURE_2D, 0);
    }
}

void GLRenderer::ClearColorBuffer (const Vector4f& rgba)
{
    glClearColor (rgba[0], rgba[1], rgba[2], rgba[3]);
    glClear (GL_COLOR_BUFFER_BIT);
}

void GLRenderer::ClearDepthBuffer ()
{
    glClearDepth (1.0f);
    glClear (GL_DEPTH_BUFFER_BIT);
}

void GLRenderer::ClearStencilBuffer ()
{
    glClearStencil (0);
    glClear (GL_STENCIL_BUFFER_BIT);
}

void GLRenderer::SetDepthTestEnabled (bool value)
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

void GLRenderer::SetDepthTestFunction (EDepthTestFunction testFunction)
{
    auto GLFun = [](EDepthTestFunction f)
    {
        switch (f)   
        {
            case EDepthTestFunction::FUNCTION_ALWAYS: return GL_ALWAYS; break;
            case EDepthTestFunction::FUNCTION_EQUAL: return GL_EQUAL; break;
            case EDepthTestFunction::FUNCTION_GEQUAL: return GL_GEQUAL; break;
            case EDepthTestFunction::FUNCTION_GREATER: return GL_GREATER; break;
            case EDepthTestFunction::FUNCTION_LEQUAL: return GL_LEQUAL; break;
            case EDepthTestFunction::FUNCTION_LESS: return GL_LESS; break;
            case EDepthTestFunction::FUNCTION_NEVER: return GL_NEVER; break;
            case EDepthTestFunction::FUNCTION_NOTEQUAL: return GL_NOTEQUAL; break;
            default: return GL_ALWAYS; break;
        }
    };

    glDepthFunc (GLFun (testFunction));
}

void GLRenderer::SetFaceCullingEnabled (bool value)
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

void GLRenderer::SetFaceCullingMode (EFaceCullingFace face, EFaceCullingDirection direction)
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

void GLRenderer::SetMultisamplingEnabled (bool value)
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

void GLRenderer::SetStencilTestEnabled (bool value)
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

void GLRenderer::SetStencilTestFunction (EStencilTestFunction testFunction, int testValue)
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

void GLRenderer::SetStencilTestOperation (EStencilTestOperation sFail, EStencilTestOperation dpFail, EStencilTestOperation dpPass)
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

void GLRenderer::InitializeShaderLibrary ()
{
    std::shared_ptr<GLShaderProgram> p;

    // load standard shaders
    GetGameScene ()->GetGame ()->GetResourceManager ()->Load<GLShader> ("default_vertex_shader", "shaders/default.vs", EShaderType::VERTEX_SHADER);
    GetGameScene ()->GetGame ()->GetResourceManager ()->Load<GLShader> ("flatquad_vertex_shader", "shaders/flatquad.vs", EShaderType::VERTEX_SHADER);
    GetGameScene ()->GetGame ()->GetResourceManager ()->Load<GLShader> ("pbr_forward_fragment_shader", "shaders/pbr_forward.fs", EShaderType::FRAGMENT_SHADER);    
    GetGameScene ()->GetGame ()->GetResourceManager ()->Load<GLShader> ("pbr_deferred_geometrypass_fragment_shader", "shaders/pbr_deferred_geometrypass.fs", EShaderType::FRAGMENT_SHADER); 
    GetGameScene ()->GetGame ()->GetResourceManager ()->Load<GLShader> ("pbr_deferred_lightingpass_fragment_shader", "shaders/pbr_deferred_lightingpass.fs", EShaderType::FRAGMENT_SHADER); 
    GetGameScene ()->GetGame ()->GetResourceManager ()->Load<GLShader> ("blinnphong_forward_fragment_shader", "shaders/blinnphong_forward.fs", EShaderType::FRAGMENT_SHADER);
    GetGameScene ()->GetGame ()->GetResourceManager ()->Load<GLShader> ("blinnphong_deferred_geometrypass_fragment_shader", "shaders/blinnphong_deferred_geometrypass.fs", EShaderType::FRAGMENT_SHADER); 
    GetGameScene ()->GetGame ()->GetResourceManager ()->Load<GLShader> ("blinnphong_deferred_lightingpass_fragment_shader", "shaders/blinnphong_deferred_lightingpass.fs", EShaderType::FRAGMENT_SHADER); 
    GetGameScene ()->GetGame ()->GetResourceManager ()->Load<GLShader> ("flatquad_fragment_shader", "shaders/flatquad.fs", EShaderType::FRAGMENT_SHADER);
    GetGameScene ()->GetGame ()->GetResourceManager ()->Load<GLShader> ("post_hdr_fragment_shader", "shaders/post_hdr.fs", EShaderType::FRAGMENT_SHADER);
    GetGameScene ()->GetGame ()->GetResourceManager ()->Load<GLShader> ("post_gamma_fragment_shader", "shaders/post_gamma.fs", EShaderType::FRAGMENT_SHADER);
    GetGameScene ()->GetGame ()->GetResourceManager ()->Load<GLShader> ("post_fxaa_fragment_shader", "shaders/post_fxaa.fs", EShaderType::FRAGMENT_SHADER);
    GetGameScene ()->GetGame ()->GetResourceManager ()->Load<GLShader> ("shadowmap_vertex_shader", "shaders/shadowmap.vs", EShaderType::VERTEX_SHADER);
    GetGameScene ()->GetGame ()->GetResourceManager ()->Load<GLShader> ("shadowmap_directional_geometry_shader", "shaders/shadowmap_directional.gs", EShaderType::GEOMETRY_SHADER);
    GetGameScene ()->GetGame ()->GetResourceManager ()->Load<GLShader> ("shadowmap_fragment_shader", "shaders/shadowmap.fs", EShaderType::FRAGMENT_SHADER);
    GetGameScene ()->GetGame ()->GetResourceManager ()->Load<GLShader> ("aabb_vertex_shader", "shaders/aabb.vs", EShaderType::VERTEX_SHADER);
    GetGameScene ()->GetGame ()->GetResourceManager ()->Load<GLShader> ("aabb_fragment_shader", "shaders/aabb.fs", EShaderType::FRAGMENT_SHADER);
    if (GLUtils::GetGLSLVersion ().versionNumber >= 430)
    {
        GetGameScene ()->GetGame ()->GetResourceManager ()->Load<GLShader> ("aabb_compute_shader", "shaders/aabb.cs", EShaderType::COMPUTE_SHADER);
    }

    // PBR model (forward)
    p = Create<GLShaderProgram> ("pbr_forward_shader");
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("default_vertex_shader"));
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("pbr_forward_fragment_shader"));
    p->Link ();
    p->Use ();
    if (GLUtils::GetGLSLVersion ().versionNumber < 330)
    {
        glBindAttribLocation (p->GetProgramId (), 0, "vPosition");
        glBindAttribLocation (p->GetProgramId (), 1, "vNormal");
        glBindAttribLocation (p->GetProgramId (), 2, "vUV");
        glBindAttribLocation (p->GetProgramId (), 3, "vTangent");
        glBindAttribLocation (p->GetProgramId (), 4, "vBitangent");
    }
    if (GLUtils::GetGLSLVersion ().versionNumber < 430)
    {
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tAlbedo"), 0);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tNormal"), 1);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tMetallic"), 2);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tRoughness"), 3);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tAO"), 4);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tDirectionalShadowMap"), 5);
    }
    p->BindUniformBlock ("UniformMatricesBlock", EGlUBOType::UBO_MATRICES);
    p->BindUniformBlock ("UniformPointLightsBlock", EGlUBOType::UBO_POINTLIGHTS);
    p->BindUniformBlock ("UniformDirectionalLightsBlock", EGlUBOType::UBO_DIRECTIONALLIGHTS);
    p->BindUniformBlock ("UniformSpotLightsBlock", EGlUBOType::UBO_SPOTLIGHTS);
    p->BindUniformBlock ("UniformDirectionalLightViewMatricesBlock", EGlUBOType::UBO_DIRECTIONALLIGHTVIEWMATRICES);
    p->BindUniformBlock ("UniformBoneTransformationsBlock", EGlUBOType::UBO_BONETRANSFORMATIONS);
    GLUtils::CheckGLError (MSG_LOCATION);

    // PBR model (deferred, geometry pass)
    p = Create<GLShaderProgram> ("pbr_deferred_geometrypass_shader");
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("default_vertex_shader"));
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("pbr_deferred_geometrypass_fragment_shader"));
    p->Link ();
    p->Use ();
    if (GLUtils::GetGLSLVersion ().versionNumber < 330)
    {
        glBindAttribLocation (p->GetProgramId (), 0, "vPosition");
        glBindAttribLocation (p->GetProgramId (), 1, "vNormal");
        glBindAttribLocation (p->GetProgramId (), 2, "vUV");
        glBindAttribLocation (p->GetProgramId (), 3, "vTangent");
        glBindAttribLocation (p->GetProgramId (), 4, "vBitangent");
    }
    if (GLUtils::GetGLSLVersion ().versionNumber < 430)
    {
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tAlbedo"), 0);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tNormal"), 1);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tMetallic"), 2);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tRoughness"), 3);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tAO"), 4);
    }
    p->BindUniformBlock ("UniformMatricesBlock", EGlUBOType::UBO_MATRICES);
    p->BindUniformBlock ("UniformBoneTransformationsBlock", EGlUBOType::UBO_BONETRANSFORMATIONS);
    GLUtils::CheckGLError (MSG_LOCATION);

    // PBR model (deferred, lighting pass)
    p = Create<GLShaderProgram> ("pbr_deferred_lightingpass_shader");
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("flatquad_vertex_shader"));
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("pbr_deferred_lightingpass_fragment_shader"));
    p->Link ();
    p->Use ();
    if (GLUtils::GetGLSLVersion ().versionNumber < 330)
    {
        glBindAttribLocation(p->GetProgramId (), 0, "vPosition");
        glBindAttribLocation(p->GetProgramId (), 1, "vTextureCoordinates");
    }
    if (GLUtils::GetGLSLVersion ().versionNumber < 430)
    {
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tPosition"), 0);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tNormal"), 1);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tAlbedo"), 2);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tMetallicRoughnessAO"), 3);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tUnused"), 4);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tDirectionalShadowMap"), 5);
    }
    p->BindUniformBlock ("UniformPointLightsBlock", EGlUBOType::UBO_POINTLIGHTS);
    p->BindUniformBlock ("UniformDirectionalLightsBlock", EGlUBOType::UBO_DIRECTIONALLIGHTS);
    p->BindUniformBlock ("UniformSpotLightsBlock", EGlUBOType::UBO_SPOTLIGHTS);
    p->BindUniformBlock ("UniformDirectionalLightViewMatricesBlock", EGlUBOType::UBO_DIRECTIONALLIGHTVIEWMATRICES);  
    GLUtils::CheckGLError (MSG_LOCATION);

    // Blinn-Phong model (forward)
    p = Create<GLShaderProgram> ("blinnphong_forward_shader");
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("default_vertex_shader"));
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("blinnphong_forward_fragment_shader"));
    p->Link ();
    p->Use ();
    if (GLUtils::GetGLSLVersion ().versionNumber < 330)
    {
        glBindAttribLocation(p->GetProgramId (), 0, "vPosition");
        glBindAttribLocation(p->GetProgramId (), 1, "vNormal");
        glBindAttribLocation(p->GetProgramId (), 2, "vUV");
        glBindAttribLocation(p->GetProgramId (), 3, "vTangent");
        glBindAttribLocation(p->GetProgramId (), 4, "vBitangent");
    }
    if (GLUtils::GetGLSLVersion ().versionNumber < 430)
    {
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tDiffuse"), 0);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tNormal"), 1);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tSpecular"), 2);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tEmissive"), 3);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tDirectionalShadowMap"), 4);
    }
    p->BindUniformBlock ("UniformMatricesBlock", EGlUBOType::UBO_MATRICES);
    p->BindUniformBlock ("UniformPointLightsBlock", EGlUBOType::UBO_POINTLIGHTS);
    p->BindUniformBlock ("UniformDirectionalLightsBlock", EGlUBOType::UBO_DIRECTIONALLIGHTS);
    p->BindUniformBlock ("UniformSpotLightsBlock", EGlUBOType::UBO_SPOTLIGHTS); 
    p->BindUniformBlock ("UniformDirectionalLightViewMatricesBlock", EGlUBOType::UBO_DIRECTIONALLIGHTVIEWMATRICES);
    p->BindUniformBlock ("UniformBoneTransformationsBlock", EGlUBOType::UBO_BONETRANSFORMATIONS);
    GLUtils::CheckGLError (MSG_LOCATION);
    
    // Blinn-Phong model (deferred, geometry pass)
    p = Create<GLShaderProgram> ("blinnphong_deferred_geometrypass_shader");
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("default_vertex_shader"));
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("blinnphong_deferred_geometrypass_fragment_shader"));
    p->Link ();
    p->Use ();
    if (GLUtils::GetGLSLVersion ().versionNumber < 330)
    {
        glBindAttribLocation(p->GetProgramId (), 0, "vPosition");
        glBindAttribLocation(p->GetProgramId (), 1, "vNormal");
        glBindAttribLocation(p->GetProgramId (), 2, "vUV");
        glBindAttribLocation(p->GetProgramId (), 3, "vTangent");
        glBindAttribLocation(p->GetProgramId (), 4, "vBitangent");
    }
    if (GLUtils::GetGLSLVersion ().versionNumber < 430)
    {
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tDiffuse"), 0);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tNormal"), 1);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tSpecular"), 2);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tEmissive"), 3);
    }    
    p->BindUniformBlock ("UniformMatricesBlock", EGlUBOType::UBO_MATRICES);
    p->BindUniformBlock ("UniformBoneTransformationsBlock", EGlUBOType::UBO_BONETRANSFORMATIONS);
    GLUtils::CheckGLError (MSG_LOCATION);

    // Blinn-Phong model (deferred, lighting pass)
    p = Create<GLShaderProgram> ("blinnphong_deferred_lightingpass_shader");
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("flatquad_vertex_shader"));
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("blinnphong_deferred_lightingpass_fragment_shader"));
    p->Link ();
    p->Use ();
    if (GLUtils::GetGLSLVersion ().versionNumber < 330)
    {
        glBindAttribLocation(p->GetProgramId (), 0, "vPosition");
        glBindAttribLocation(p->GetProgramId (), 1, "vTextureCoordinates");
    }
    if (GLUtils::GetGLSLVersion ().versionNumber < 430)
    {
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tPosition"), 0);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tNormal"), 1);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tDiffuse"), 2);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tEmissive"), 3);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tSpecular"), 4);
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "tDirectionalShadowMap"), 5);
    }
    p->BindUniformBlock ("UniformPointLightsBlock", EGlUBOType::UBO_POINTLIGHTS);
    p->BindUniformBlock ("UniformDirectionalLightsBlock", EGlUBOType::UBO_DIRECTIONALLIGHTS);
    p->BindUniformBlock ("UniformSpotLightsBlock", EGlUBOType::UBO_SPOTLIGHTS);   
    p->BindUniformBlock ("UniformDirectionalLightViewMatricesBlock", EGlUBOType::UBO_DIRECTIONALLIGHTVIEWMATRICES); 
    GLUtils::CheckGLError (MSG_LOCATION);

    // Screen quad rendering
    p = Create<GLShaderProgram> ("flatquad_shader");
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("flatquad_vertex_shader"));
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("flatquad_fragment_shader"));   
    p->Link ();
    p->Use (); 
    if (GLUtils::GetGLSLVersion ().versionNumber < 330)
    {
        glBindAttribLocation (p->GetProgramId (), 0, "vPosition");
        glBindAttribLocation (p->GetProgramId (), 1, "vTextureCoordinates");
    }
    if (GLUtils::GetGLSLVersion ().versionNumber < 430)
    {
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "fScreenTexture"), 0);
    }
    GLUtils::CheckGLError (MSG_LOCATION);

    // Post-processing HDR
    p = Create<GLShaderProgram> ("post_hdr_shader");
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("flatquad_vertex_shader"));
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("post_hdr_fragment_shader"));
    p->Link ();
    p->Use ();
    if (GLUtils::GetGLSLVersion ().versionNumber < 330)
    {
        glBindAttribLocation (p->GetProgramId (), 0, "vPosition");
        glBindAttribLocation (p->GetProgramId (), 1, "vTextureCoordinates");
    }
    if (GLUtils::GetGLSLVersion ().versionNumber < 430)
    {
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "fScreenTexture"), 0);
    }
    GLUtils::CheckGLError (MSG_LOCATION);

    // Post-processing gamma
    p = Create<GLShaderProgram> ("post_gamma_shader");
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("flatquad_vertex_shader"));
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("post_gamma_fragment_shader"));   
    p->Link ();
    p->Use (); 
    if (GLUtils::GetGLSLVersion ().versionNumber < 330)
    {
        glBindAttribLocation (p->GetProgramId (), 0, "vPosition");
        glBindAttribLocation (p->GetProgramId (), 1, "vTextureCoordinates");
    }
    if (GLUtils::GetGLSLVersion ().versionNumber < 430)
    {
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "fScreenTexture"), 0);
    }
    GLUtils::CheckGLError (MSG_LOCATION);

    // Post-processing fxaa
    p = Create<GLShaderProgram> ("post_fxaa_shader");
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("flatquad_vertex_shader"));
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("post_fxaa_fragment_shader"));   
    p->Link ();
    p->Use (); 
    if (GLUtils::GetGLSLVersion ().versionNumber < 330)
    {
        glBindAttribLocation (p->GetProgramId (), 0, "vPosition");
        glBindAttribLocation (p->GetProgramId (), 1, "vTextureCoordinates");
    }
    if (GLUtils::GetGLSLVersion ().versionNumber < 430)
    {
        glUniform1i (glGetUniformLocation (p->GetProgramId (), "fScreenTexture"), 0);
    }
    GLUtils::CheckGLError (MSG_LOCATION);

    // Shadow map (directional)
    p = Create<GLShaderProgram> ("shadowmap_directional_shader");
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("shadowmap_vertex_shader"));
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("shadowmap_directional_geometry_shader"));
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("shadowmap_fragment_shader"));
    p->Link ();
    p->Use (); 
    if (GLUtils::GetGLSLVersion ().versionNumber < 330)
    {
        glBindAttribLocation (p->GetProgramId (), 0, "vPosition");
    }
    p->BindUniformBlock ("UniformDirectionalLightViewMatricesBlock", EGlUBOType::UBO_DIRECTIONALLIGHTVIEWMATRICES);
    p->BindUniformBlock ("UniformBoneTransformationsBlock", EGlUBOType::UBO_BONETRANSFORMATIONS);
    GLUtils::CheckGLError (MSG_LOCATION);

    // AABB rendering
    p = Create<GLShaderProgram> ("aabb_shader");
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("aabb_vertex_shader"));
    p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("aabb_fragment_shader"));
    p->Link ();
    p->Use ();
    if (GLUtils::GetGLSLVersion ().versionNumber < 330)
    {
        glBindAttribLocation (p->GetProgramId (), 0, "vPosition");
    }
    p->BindUniformBlock ("UniformMatricesBlock", EGlUBOType::UBO_MATRICES);
    GLUtils::CheckGLError (MSG_LOCATION);

    if (GLUtils::GetGLSLVersion ().versionNumber >= 430)
    {  
        // AABB compute shader
        p = Create<GLShaderProgram> ("aabb_compute_shader");
        p->AttachShader (GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<GLShader>("aabb_compute_shader"));
        p->Link ();
        p->Use ();
        p->BindUniformBlock ("UniformBoneTransformationsBlock", EGlUBOType::UBO_BONETRANSFORMATIONS);
        p->BindShaderStorageBlock ("VertexBufferBlock", EGlSSBOType::SSBO_VERTICES);  
        p->BindShaderStorageBlock ("BoneIndicesBufferBlock", EGlSSBOType::SSBO_BONEINDICES);
        p->BindShaderStorageBlock ("BoneWeightsBufferBlock", EGlSSBOType::SSBO_BONEWEIGHTS);
        p->BindShaderStorageBlock ("AABBBufferBlock", EGlSSBOType::SSBO_AABB);
        GLUtils::CheckGLError (MSG_LOCATION);
    }

}

void GLRenderer::InitializeMatrixUniformBuffers ()
{
    // create uniform buffer for view and projection matrices
    glGenBuffers (1, &m_uniformBuffers->UBO[UBO_MATRICES]);
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_MATRICES]);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGlUniformMatrixBuffer), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, static_cast<int>(EGlUBOType::UBO_MATRICES), m_uniformBuffers->UBO[UBO_MATRICES]);

    GLUtils::CheckGLError (MSG_LOCATION);
}

void GLRenderer::LoadMatrixUniformBuffers (std::shared_ptr<Camera> camera)
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

void GLRenderer::DeinitializeMatrixUniformBuffers ()
{
    glDeleteBuffers (1, &m_uniformBuffers->UBO[UBO_MATRICES]);
}

void GLRenderer::InitializeLightViewMatrixUniformBuffers ()
{
    // create unform buffers for light view transforms
    glGenBuffers (1, &m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTVIEWMATRICES]);
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTVIEWMATRICES]);
    glBufferData (GL_UNIFORM_BUFFER, 16 * sizeof (GLfloat) * CILANTRO_MAX_DIRECTIONAL_LIGHTS, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, static_cast<int>(EGlUBOType::UBO_DIRECTIONALLIGHTVIEWMATRICES), m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTVIEWMATRICES]);

    GLUtils::CheckGLError (MSG_LOCATION);
}

void GLRenderer::LoadLightViewMatrixUniformBuffers ()
{
    auto frustumVertices = GetGameScene ()->GetActiveCamera ()->GetFrustumVertices (m_width, m_height);
    AABB sceneAABB = GetGameScene ()->GetGameObjectManager ()->GetByName<GameObject> ("root")->GetHierarchyAABB ();

    // calculate and load lightview matrix for each directional light
    for (auto&& light : m_directionalLights)
    {
        // generate matrix
        Matrix4f lightViewProjection = GetGameScene ()->GetGameObjectManager ()->GetByHandle<DirectionalLight> (light.first)->GenLightViewProjectionMatrix (frustumVertices, sceneAABB);

        // copy to buffer
        std::memcpy (m_uniformLightViewMatrixBuffer->directionalLightView + light.second * 16, Mathf::Transpose (lightViewProjection)[0], 16 * sizeof (GLfloat));
    }

    // load to GPU - directional light view
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTVIEWMATRICES]);
    glBufferSubData (GL_UNIFORM_BUFFER, 0, 16 * sizeof (GLfloat) * m_uniformDirectionalLightBuffer->directionalLightCount, m_uniformLightViewMatrixBuffer->directionalLightView);
    glBindBuffer (GL_UNIFORM_BUFFER, 0);

}

void GLRenderer::DeinitializeLightViewMatrixUniformBuffers ()
{
    glDeleteBuffers (1, &m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTVIEWMATRICES]);
}

void GLRenderer::InitializeObjectBuffers ()
{
    // create and load object buffers for all existing objects
    for (auto&& gameObject : GetGameScene ()->GetGameObjectManager ())
    {
        // load buffers for MeshObject only
        if (std::dynamic_pointer_cast<MeshObject> (gameObject) != nullptr)
        {
            gameObject->OnUpdate (*this);
        }      
    }
}

void GLRenderer::DeinitializeObjectBuffers ()
{
    for (auto&& buffer : m_sceneGeometryBuffers)
    {
        glDeleteBuffers (1, &buffer.second->boneTransformationsUBO);
        
        if (GLUtils::GetGLSLVersion ().versionNumber >= 430)
        {
            glDeleteBuffers (1, &buffer.second->vertexPositionsSSBO);
            glDeleteBuffers (1, &buffer.second->boneIndicesSSBO);
            glDeleteBuffers (1, &buffer.second->boneWeightsSSBO);
            glDeleteBuffers (1, &buffer.second->aabbSSBO);
        }    

        glDeleteBuffers (CILANTRO_VBO_COUNT, buffer.second->VBO);
        glDeleteBuffers (1, &buffer.second->EBO);
        glDeleteVertexArrays (1, &buffer.second->VAO);
    }
}

void GLRenderer::InitializeQuadGeometryBuffer ()
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

    glGenVertexArrays (1, &m_surfaceGeometryBuffer->VAO);    
    glBindVertexArray (m_surfaceGeometryBuffer->VAO);

    glGenBuffers (1, &m_surfaceGeometryBuffer->VBO[EGlVBOType::VBO_VERTICES]);
    glGenBuffers (1, &m_surfaceGeometryBuffer->VBO[EGlVBOType::VBO_UVS]);

    glBindBuffer (GL_ARRAY_BUFFER, m_surfaceGeometryBuffer->VBO[EGlVBOType::VBO_VERTICES]);
    glBufferData (GL_ARRAY_BUFFER, sizeof (quadVertices), &quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer (EGlVBOType::VBO_VERTICES, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer (GL_ARRAY_BUFFER, m_surfaceGeometryBuffer->VBO[EGlVBOType::VBO_UVS]);
    glBufferData (GL_ARRAY_BUFFER, sizeof (quadUV), &quadUV, GL_STATIC_DRAW);
    glVertexAttribPointer (EGlVBOType::VBO_UVS, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glEnableVertexAttribArray (EGlVBOType::VBO_VERTICES);
    glEnableVertexAttribArray (EGlVBOType::VBO_UVS);

    glGenBuffers (1, &m_surfaceGeometryBuffer->EBO);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_surfaceGeometryBuffer->EBO);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (quadIndices), &quadIndices, GL_STATIC_DRAW);

    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindVertexArray (0);    

    m_surfaceGeometryBuffer->indexCount = 6;

    GLUtils::CheckGLError (MSG_LOCATION);
}

void GLRenderer::DeinitializeQuadGeometryBuffer ()
{
    glDeleteVertexArrays(1, &m_surfaceGeometryBuffer->VAO);
    glDeleteBuffers(1, &m_surfaceGeometryBuffer->VBO[EGlVBOType::VBO_VERTICES]);
}

void GLRenderer::InitializeLightUniformBuffers ()
{
    m_uniformPointLightBuffer->pointLightCount = 0;
    m_uniformSpotLightBuffer->spotLightCount = 0;
    m_uniformDirectionalLightBuffer->directionalLightCount = 0;

    // create uniform buffer for point lights
    glGenBuffers (1, &m_uniformBuffers->UBO[UBO_POINTLIGHTS]);
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_POINTLIGHTS]);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGlUniformPointLightBuffer), m_uniformPointLightBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, static_cast<int>(EGlUBOType::UBO_POINTLIGHTS), m_uniformBuffers->UBO[UBO_POINTLIGHTS]);

    // create uniform buffer for directional lights
    glGenBuffers (1, &m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTS]);
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTS]);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGlUniformDirectionalLightBuffer), m_uniformDirectionalLightBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, static_cast<int>(EGlUBOType::UBO_DIRECTIONALLIGHTS), m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTS]);

    // create uniform buffer for spot lights
    glGenBuffers (1, &m_uniformBuffers->UBO[UBO_SPOTLIGHTS]);
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformBuffers->UBO[UBO_SPOTLIGHTS]);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGlUniformSpotLightBuffer), m_uniformSpotLightBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, static_cast<int>(EGlUBOType::UBO_SPOTLIGHTS), m_uniformBuffers->UBO[UBO_SPOTLIGHTS]);

    // scan objects vector for lights and populate light buffers
    for (auto&& gameObject : GetGameScene ()->GetGameObjectManager ())
    {
        if (std::dynamic_pointer_cast<Light> (gameObject) != nullptr)
        {
            gameObject->OnUpdate (*this);
        }
    }

}

void GLRenderer::DeinitializeLightUniformBuffers ()
{
    // delete all light buffers
    glDeleteBuffers (1, &m_uniformBuffers->UBO[UBO_POINTLIGHTS]);
    glDeleteBuffers (1, &m_uniformBuffers->UBO[UBO_DIRECTIONALLIGHTS]);
    glDeleteBuffers (1, &m_uniformBuffers->UBO[UBO_SPOTLIGHTS]);
}

void GLRenderer::UpdateLightBufferRecursive (handle_t objectHandle)
{
    auto light = GetGameScene ()->GetGameObjectManager ()->GetByHandle<GameObject> (objectHandle);

    if (std::dynamic_pointer_cast<Light>(light) != nullptr)
    {
        light->OnUpdate (*this);
    }

    for (auto&& childObject : light->GetChildren ())
    {
        UpdateLightBufferRecursive (childObject.lock ()->GetHandle ());
    }

}

void GLRenderer::RenderGeometryBuffer (SGlGeometryBuffers* buffer, GLuint type)
{
    // bind
    glBindVertexArray (buffer->VAO);
    
    // draw
    glDrawElements (type, static_cast<GLsizei> (buffer->indexCount) * sizeof (GLuint), GL_UNSIGNED_INT, 0);
    
    // unbind
    glBindVertexArray (0);
}

} // namespace cilantro
