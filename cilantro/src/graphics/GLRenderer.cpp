#include "graphics/GLRenderer.h"
#include "graphics/GLUtils.h"
#include "graphics/GLShader.h"
#include "graphics/GLShaderProgram.h"
#include "graphics/GLShaderLibrary.h"
#include "graphics/GLCameraBuffer.h"
#include "graphics/GLLightBuffers.h"
#include "graphics/GLMaterialBindings.h"
#include "graphics/GLFramebuffer.h"
#include "graphics/GLMultisampleFramebuffer.h"
#include "graphics/SurfaceRenderStage.h"
#include "graphics/DeferredGeometryRenderStage.h"
#include "graphics/DeferredLightingRenderStage.h"
#include "graphics/ForwardGeometryRenderStage.h"

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
    m_cameraBuffer = std::make_unique<GLCameraBuffer> ();
    m_lightBuffers = std::make_unique<GLLightBuffers> ();
    m_materialBindings = std::make_unique<GLMaterialBindings> ();
}

GLRenderer::~GLRenderer ()
{
    for (auto&& objectBuffer : m_sceneGeometryBuffers)
    {
        delete objectBuffer.second;
    }

    delete m_surfaceGeometryBuffer;
}

void GLRenderer::Initialize ()
{    
    Renderer::Initialize ();

    GLUtils::PrintGLInfo ();
    GLUtils::PrintGLExtensions ();

    m_shaderLibrary = std::make_unique<GLShaderLibrary> (GetGameScene ()->GetGame ()->GetResourceManager (), m_shaderProgramManager);
    m_shaderLibrary->Initialize ();
    InitializeQuadGeometryBuffer ();
    InitializeObjectBuffers ();
    m_cameraBuffer->Initialize ();
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
    m_cameraBuffer->Deinitialize ();
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
    if (m_materialBindings->Bind (meshObject->GetMaterial ()))
    {
        // bind shadow maps (if exist)
        if (m_isShadowMapping && (GetCurrentRenderStage ()->GetLinkedDepthTextureArrayFramebuffer ()) != nullptr)
        {
            if (GetCurrentRenderStage ()->GetLinkedDepthTextureArrayFramebuffer ()->IsDepthTextureArrayEnabled ())
            {
                GetCurrentRenderStage ()->GetLinkedDepthTextureArrayFramebuffer ()->BindFramebufferDepthTextureArrayAsColor (CILANTRO_SHADOW_MAP_BINDING);
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

    // set shadow map uniform (if shadow mapping is enabled)
    // this is only required for forward rendering, because deferred rendering uses a different shader program for lighting pass (DeferredLightingRenderStage)
    if (!m_isDeferredRendering)
    {
        geometryShaderProgram->SetUniformInt ("shadowMapEnabled", m_isShadowMapping ? 1 : 0);
    }
    
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
    m_materialBindings->Update (material, textureUnit);
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
            auto q = Create <DeferredLightingRenderStage> ("deferred_lighting_" + shaderProgramName);
            q->SetShaderProgram (shaderProgramName);
            q->SetStencilTestEnabled (true)->SetStencilTest (EStencilTestFunction::FUNCTION_EQUAL, static_cast<int> (shaderProgramHandle));
            q->SetClearColorOnFrameEnabled (true);
            q->SetClearDepthOnFrameEnabled (false);
            q->SetClearStencilOnFrameEnabled (false);
            q->SetDepthTestEnabled (false);
            q->SetColorAttachmentsFramebufferLink (m_isShadowMapping ? EPipelineLink::LINK_SECOND : EPipelineLink::LINK_FIRST);
            q->SetDepthStencilFramebufferLink (m_isShadowMapping ? EPipelineLink::LINK_SECOND : EPipelineLink::LINK_FIRST);
            q->SetDepthTextureArrayFramebufferLink (m_isShadowMapping ? EPipelineLink::LINK_FIRST : EPipelineLink::LINK_CURRENT);
            q->SetDepthCubeMapArrayFramebufferLink (m_isShadowMapping ? EPipelineLink::LINK_FIRST : EPipelineLink::LINK_CURRENT);
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

                auto stage = m_renderStageManager->GetByHandle<DeferredLightingRenderStage> (stageHandle);
                stage->SetClearColorOnFrameEnabled (false);
                stage->SetFramebufferEnabled (false);
            }

        }
    }
}

void GLRenderer::Update (std::shared_ptr<PointLight> pointLight)
{
    if (m_lightBuffers->Update (pointLight))
    {
        m_shaderLibrary->OnPointLightAdded (GetDirectionalLightCount (), GetSpotLightCount (), GetPointLightCount ());
    }
}

void GLRenderer::Update (std::shared_ptr<DirectionalLight> directionalLight)
{
    if (m_lightBuffers->Update (directionalLight))
    {
        m_shaderLibrary->OnDirectionalLightAdded (GetDirectionalLightCount (), GetSpotLightCount (), GetPointLightCount ());
    }
}

void GLRenderer::Update (std::shared_ptr<SpotLight> spotLight)
{
    if (m_lightBuffers->Update (spotLight))
    {
        m_shaderLibrary->OnSpotLightAdded (GetDirectionalLightCount (), GetSpotLightCount (), GetPointLightCount ());
    }
}

void GLRenderer::UpdateCameraBuffers (std::shared_ptr<Camera> camera)
{
    m_cameraBuffer->Update (camera, m_width, m_height);
}

void GLRenderer::UpdateLightViewBuffers ()
{
    auto frustumVertices = GetGameScene ()->GetActiveCamera ()->GetFrustumVertices (m_width, m_height);
    AABB sceneAABB = GetGameScene ()->GetGameObjectManager ()->GetByName<GameObject> ("root")->GetHierarchyAABB ();

    m_lightBuffers->LoadLightViewMatrices (GetGameScene ()->GetGameObjectManager (), frustumVertices, sceneAABB);
}

size_t GLRenderer::GetPointLightCount () const
{
    return m_lightBuffers->GetPointLightCount ();
}

size_t GLRenderer::GetDirectionalLightCount () const
{
    return m_lightBuffers->GetDirectionalLightCount ();
}

size_t GLRenderer::GetSpotLightCount () const
{
    return m_lightBuffers->GetSpotLightCount ();
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
    m_lightBuffers->Initialize ();

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
    m_lightBuffers->Deinitialize ();
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
