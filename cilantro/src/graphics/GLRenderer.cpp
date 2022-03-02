#include "graphics/GLRenderer.h"
#include "graphics/GLShader.h"
#include "graphics/GLShaderProgram.h"
#include "graphics/GLFramebuffer.h"
#if (CILANTRO_GL_VERSION > 140)
#include "graphics/GLMultisampleFramebuffer.h"
#endif
#include "graphics/QuadRenderStage.h"
#include "glad/glad.h"
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

GLRenderer::GLRenderer (GameScene* gameScene, unsigned int width, unsigned int height, bool isDeferred) 
    : Renderer (gameScene, width, height, isDeferred)
{
    quadGeometryBuffer = new SGLGeometryBuffers ();
    uniformBuffers = new SGLUniformBuffers ();
    uniformMatrixBuffer = new SGLUniformMatrixBuffer ();
    uniformPointLightBuffer = new SGLUniformPointLightBuffer ();
    uniformDirectionalLightBuffer = new SGLUniformDirectionalLightBuffer ();
    uniformSpotLightBuffer = new SGLUniformSpotLightBuffer ();
}

GLRenderer::~GLRenderer ()
{
    for (auto&& objectBuffer : sceneGeometryBuffers)
    {
        delete objectBuffer.second;
    }

    delete quadGeometryBuffer;
    delete uniformBuffers;
    delete uniformMatrixBuffer;
    delete uniformPointLightBuffer;
    delete uniformDirectionalLightBuffer;
    delete uniformSpotLightBuffer;
}

void GLRenderer::Initialize ()
{
    InitializeShaderLibrary ();
    InitializeQuadGeometryBuffer ();

    Renderer::Initialize ();
}

void GLRenderer::Deinitialize ()
{
    Renderer::Deinitialize ();

    DeinitializeQuadGeometryBuffer ();
}

void GLRenderer::RenderFrame ()
{
    Renderer::RenderFrame ();

    //CheckGLError (MSG_LOCATION);
}

void GLRenderer::RenderGeometryBuffer (SGeometryBuffers* buffer)
{
    SGLGeometryBuffers* glBuffer = static_cast<SGLGeometryBuffers*>(buffer);

    // bind
    glBindVertexArray (glBuffer->VAO);    

    // draw
    glViewport (0, 0, width, height);
    glDrawArrays (GL_TRIANGLES, 0, glBuffer->vertexCount);

    // unbind
    glBindTexture (GL_TEXTURE_2D, 0);
    glBindVertexArray (0);
    glBindFramebuffer (GL_FRAMEBUFFER, 0);
}

Framebuffer* GLRenderer::CreateFramebuffer (unsigned int rgbTextures, unsigned int rgbaTextures, bool multisampleEnabled)
{
    Framebuffer* framebuffer;

    if (multisampleEnabled)
    {
#if (CILANTRO_GL_VERSION <= 140)
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "OpenGL 3.2 required for multisample framebuffers";
#else
        framebuffer = new GLMultisampleFramebuffer (this->width, this->height, rgbTextures, rgbaTextures);
#endif
    }
    else
    {
        framebuffer = new GLFramebuffer (this->width, this->height, rgbTextures, rgbaTextures);
    }
    
    framebuffer->Initialize ();

    return framebuffer;
}

void GLRenderer::BindDefaultFramebuffer ()
{
    glBindFramebuffer (GL_FRAMEBUFFER, (GLint) 0);
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

void GLRenderer::SetFaceCullingEnabled (bool value)
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

void GLRenderer::SetStencilTestFunction (StencilTestFunction testFunction, int testValue)
{
    GLuint glStencilFunction;

    switch (testFunction)   
    {
        case StencilTestFunction::FUNCTION_ALWAYS: glStencilFunction = GL_ALWAYS; break;
        case StencilTestFunction::FUNCTION_EQUAL: glStencilFunction = GL_EQUAL; break;
        case StencilTestFunction::FUNCTION_GEQUAL: glStencilFunction = GL_GEQUAL; break;
        case StencilTestFunction::FUNCTION_GREATER: glStencilFunction = GL_GREATER; break;
        case StencilTestFunction::FUNCTION_LEQUAL: glStencilFunction = GL_LEQUAL; break;
        case StencilTestFunction::FUNCTION_LESS: glStencilFunction = GL_LESS; break;
        case StencilTestFunction::FUNCTION_NEVER: glStencilFunction = GL_NEVER; break;
        case StencilTestFunction::FUNCTION_NOTEQUAL: glStencilFunction = GL_NOTEQUAL; break;
        default: glStencilFunction = GL_ALWAYS; break;
    }

    glStencilFunc (glStencilFunction, testValue, 0xff);
}

void GLRenderer::Draw (MeshObject& meshObject)
{
    GLuint eyePositionId;
    GLuint modelMatrixId;
    GLuint normalMatrixId;
    GLuint boneTransformationArrayId;
    GLuint shaderProgramId;
    GLuint uniformId;

    Material& objM = meshObject.GetMaterial ();

    // get shader program for rendered meshobject (geometry pass)
    GLShaderProgram& geometryShaderProgram = shaderPrograms.GetByName<GLShaderProgram> (
        m_IsDeferred
        ? objM.GetDeferredGeometryPassShaderProgram ()
        : objM.GetForwardShaderProgram ()
    );
    geometryShaderProgram.Use ();
    shaderProgramId = geometryShaderProgram.GetProgramId ();

    // bind textures for active material
    if (materialTextureUnits.find(meshObject.GetMaterial ().GetHandle ()) != materialTextureUnits.end ())
    {
        SGLMaterialTextureUnits* u = static_cast<SGLMaterialTextureUnits*>(materialTextureUnits[meshObject.GetMaterial ().GetHandle ()]);

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
        glUniform3fv (eyePositionId, 1, &gameScene->GetActiveCamera ()->GetPosition ()[0]);
    }

    // set bone transformation matrix array uniform
    boneTransformationArrayId = glGetUniformLocation (shaderProgramId, "mBoneTransformations");
    if (boneTransformationArrayId != GL_INVALID_INDEX)
    {
        glUniformMatrix4fv (boneTransformationArrayId, CILANTRO_MAX_BONES, GL_TRUE, meshObject.GetBoneTransformationsMatrixArray ());
    }

    // get shader program for rendered meshobject (lighting pass)
    if (m_IsDeferred)
    {
        GLShaderProgram& lightingShaderProgram = shaderPrograms.GetByName<GLShaderProgram>(objM.GetDeferredLightingPassShaderProgram ());
        lightingShaderProgram.Use ();
        shaderProgramId = lightingShaderProgram.GetProgramId ();

        // get camera position in world space and set uniform value (this needs to be done again for deferred lighting shader program)
        eyePositionId = glGetUniformLocation (shaderProgramId, "eyePosition");
        if (eyePositionId != GL_INVALID_INDEX)
        {
            glUniform3fv (eyePositionId, 1, &gameScene->GetActiveCamera ()->GetPosition ()[0]);
        }

    }
    
    // draw mesh
    SGLGeometryBuffers* b = static_cast<SGLGeometryBuffers*>(sceneGeometryBuffers[meshObject.GetHandle ()]);
    geometryShaderProgram.Use ();
    glBindVertexArray (b->VAO);
    glDrawElements (GL_TRIANGLES, meshObject.GetMesh ().GetIndexCount (), GL_UNSIGNED_INT, 0);
    glBindVertexArray (0);
}

void GLRenderer::Update (MeshObject& meshObject)
{
    unsigned int objectHandle = meshObject.GetHandle ();

    // check of object's buffers are already initialized
    auto find = sceneGeometryBuffers.find (objectHandle);

    if (find == sceneGeometryBuffers.end ())
    {
        // it is a new object, so generate buffers 
        SGLGeometryBuffers* b = new SGLGeometryBuffers ();
        sceneGeometryBuffers.insert ({ objectHandle, b });

        // generate and bind Vertex Array Object (VAO)
        glGenVertexArrays (1, &b->VAO);
        glBindVertexArray (b->VAO);

        // generate vertex buffer
        glGenBuffers (1, &b->VBO[EVBOType::VBO_VERTICES]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EVBOType::VBO_VERTICES]);
        // location = 0 (vertex position)
        glVertexAttribPointer (EVBOType::VBO_VERTICES, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate normals buffer
        glGenBuffers (1, &b->VBO[EVBOType::VBO_NORMALS]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EVBOType::VBO_NORMALS]);
        // location = 1 (vertex normal)
        glVertexAttribPointer (EVBOType::VBO_NORMALS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate uv buffer
        glGenBuffers (1, &b->VBO[EVBOType::VBO_UVS]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EVBOType::VBO_UVS]);
        // location = 2 (vertex uv)
        glVertexAttribPointer (EVBOType::VBO_UVS, 2, GL_FLOAT, GL_FALSE, 2 * sizeof (float), (GLvoid*)0);

        // generate tangents buffer
        glGenBuffers (1, &b->VBO[EVBOType::VBO_TANGENTS]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EVBOType::VBO_TANGENTS]);
        // location = 3 (vertex tangent)
        glVertexAttribPointer (EVBOType::VBO_TANGENTS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate bitangents buffer
        glGenBuffers (1, &b->VBO[EVBOType::VBO_BITANGENTS]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EVBOType::VBO_BITANGENTS]);
        // location = 4 (vertex bitangent)
        glVertexAttribPointer (EVBOType::VBO_BITANGENTS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate bone index
        glGenBuffers (1, &b->VBO[EVBOType::VBO_BONES]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EVBOType::VBO_BONES]);
        // location = 5 (bone indices)
        glVertexAttribIPointer (EVBOType::VBO_BONES, CILANTRO_MAX_BONE_INFLUENCES, GL_INT, CILANTRO_MAX_BONE_INFLUENCES * sizeof (int), (GLvoid*)0);

        // generate bone weight buffer
        glGenBuffers (1, &b->VBO[EVBOType::VBO_BONEWEIGHTS]);
        glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EVBOType::VBO_BONEWEIGHTS]);
        // location = 6 (bone weights)
        glVertexAttribPointer (EVBOType::VBO_BONEWEIGHTS, CILANTRO_MAX_BONE_INFLUENCES, GL_FLOAT, GL_FALSE, CILANTRO_MAX_BONE_INFLUENCES * sizeof (int), (GLvoid*)0);

        // generate index buffer
        glGenBuffers (1, &b->EBO);
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, b->EBO);

        // enable VBO arrays
        glEnableVertexAttribArray (EVBOType::VBO_VERTICES);
        glEnableVertexAttribArray (EVBOType::VBO_NORMALS);
        glEnableVertexAttribArray (EVBOType::VBO_UVS);
        glEnableVertexAttribArray (EVBOType::VBO_TANGENTS);
        glEnableVertexAttribArray (EVBOType::VBO_BITANGENTS);
        glEnableVertexAttribArray (EVBOType::VBO_BONES);
        glEnableVertexAttribArray (EVBOType::VBO_BONEWEIGHTS);

        // unbind VAO
        glBindVertexArray (0);
    }

    // resize buffers and load data
    SGLGeometryBuffers* b = static_cast<SGLGeometryBuffers*>(sceneGeometryBuffers[objectHandle]);

    // bind Vertex Array Object (VAO)
    glBindVertexArray (b->VAO);

    // load vertex buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EVBOType::VBO_VERTICES]);
    glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 3, meshObject.GetMesh ().GetVerticesData (), GL_STATIC_DRAW);

    // load normals buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EVBOType::VBO_NORMALS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 3, meshObject.GetMesh ().GetNormalsData (), GL_STATIC_DRAW);
    
    // load uv buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EVBOType::VBO_UVS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 2, meshObject.GetMesh ().GetUVData (), GL_STATIC_DRAW);

    // load tangents buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EVBOType::VBO_TANGENTS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 3, meshObject.GetMesh ().GetTangentData (), GL_STATIC_DRAW);

    // load bitangents buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EVBOType::VBO_BITANGENTS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 3, meshObject.GetMesh ().GetBitangentData (), GL_STATIC_DRAW);

    // load bone index buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EVBOType::VBO_BONES]);
    glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (int) * CILANTRO_MAX_BONE_INFLUENCES, meshObject.GetMesh ().GetBoneIndicesData (), GL_STATIC_DRAW);

    // load bone weight buffer
    glBindBuffer (GL_ARRAY_BUFFER, b->VBO[EVBOType::VBO_BONEWEIGHTS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * CILANTRO_MAX_BONE_INFLUENCES, meshObject.GetMesh ().GetBoneWeightsData (), GL_STATIC_DRAW);

    // load index buffer
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, b->EBO);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, meshObject.GetMesh ().GetIndexCount () * sizeof (unsigned int), meshObject.GetMesh ().GetFacesData (), GL_STATIC_DRAW);

    // unbind VAO
    glBindVertexArray (0);

}

void GLRenderer::Update (Material& material, unsigned int textureUnit)
{
    unsigned int materialHandle = material.GetHandle ();
    GLuint texture;
    GLuint format;

    texture_map_t& textures = material.GetTexturesMap ();

    // check if material already exists
    auto find = materialTextureUnits.find (materialHandle);

    if (find == materialTextureUnits.end ())
    {
        materialTextureUnits.insert ({ materialHandle, new SGLMaterialTextureUnits () });
        
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
            
            static_cast<SGLMaterialTextureUnits*>(materialTextureUnits[materialHandle])->textureUnits[unit] = texture;
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

        glBindTexture (GL_TEXTURE_2D, static_cast<SGLMaterialTextureUnits*>(materialTextureUnits[materialHandle])->textureUnits[unit]);
        glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D (GL_TEXTURE_2D, 0, format, tPtr->GetWidth (), tPtr->GetHeight (), 0, format, GL_UNSIGNED_BYTE, tPtr->Data ());
        glGenerateMipmap (GL_TEXTURE_2D);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture (GL_TEXTURE_2D, 0);
    }

}

void GLRenderer::Update (Material& material)
{
    handle_t shaderProgramHandle = shaderPrograms.GetByName<ShaderProgram>(material.GetDeferredLightingPassShaderProgram ()).GetHandle ();
    std::string shaderProgramName = material.GetDeferredLightingPassShaderProgram ();

    if (m_IsDeferred)
    {

        // add material's shader program to set of used shader programs handles
        // add lighting deferred pass renderStages for each program
        if (lightingShaders.find (shaderProgramHandle) == lightingShaders.end ())
        {
            // first rotate the pipeline to the left so that geometry stage is last
            RotateRenderPipelineLeft ();

            lightingShaderStagesCount++;
            lightingShaders.insert (shaderProgramHandle);
            QuadRenderStage& p = AddRenderStage <QuadRenderStage> ("deferredLightingStage_" + shaderProgramName);
            p.SetShaderProgram (shaderProgramName);
            p.SetStencilTestEnabled (true).SetStencilTest (StencilTestFunction::FUNCTION_EQUAL, shaderProgramHandle);
            p.SetClearOnFrameEnabled (true);
            p.SetDepthTestEnabled (false);
            p.SetPipelineFramebufferInputLink (PipelineLink::LINK_FIRST);
            p.SetPipelineRenderbufferLink (PipelineLink::LINK_FIRST);
            p.SetPipelineFramebufferDrawLink (PipelineLink::LINK_SECOND);
            p.SetFramebufferEnabled (true);

            // rotate pipeline twice to the right, so that ultimately geometry stage is first and newly added stage is second
            RotateRenderPipelineRight ();
            RotateRenderPipelineRight ();
            
            // update flags of other deferred lighting stages (if present)
            if (lightingShaderStagesCount > 1)
            {
                handle_t stageHandle = GetRenderPipeline ()[2];

                QuadRenderStage& stage = renderStages.GetByHandle<QuadRenderStage> (stageHandle);
                stage.SetClearOnFrameEnabled (false);
                stage.SetFramebufferEnabled (false);
            }

            // update pipeline links of 1st stage following deferred lighting stages
            if (renderPipeline.size () > lightingShaderStagesCount + 1)
            {
                handle_t stageHandle = renderPipeline[lightingShaderStagesCount + 1];

                RenderStage& stage = renderStages.GetByHandle<RenderStage> (stageHandle);
                stage.SetPipelineFramebufferInputLink (PipelineLink::LINK_SECOND);
                stage.SetPipelineRenderbufferLink (PipelineLink::LINK_CURRENT);
                stage.SetPipelineFramebufferDrawLink (PipelineLink::LINK_CURRENT);
            }
        }
    }
}

void GLRenderer::LoadMatrixUniformBuffers ()
{
    SGLUniformBuffers* buffers = static_cast<SGLUniformBuffers*>(uniformBuffers);
    SGLUniformMatrixBuffer* mBuffer = static_cast<SGLUniformMatrixBuffer*>(uniformMatrixBuffer);

    // get active camera of rendered scene
    Camera* activeCamera = gameScene->GetActiveCamera ();

    if (activeCamera == nullptr)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "No active camera found";
    }

    // load view matrix
    std::memcpy (mBuffer->viewMatrix, Mathf::Transpose (activeCamera->GetViewMatrix ())[0], 16 * sizeof (GLfloat));

    // load projection matrix
    std::memcpy (mBuffer->projectionMatrix, Mathf::Transpose (activeCamera->GetProjectionMatrix (width, height))[0], 16 * sizeof (GLfloat));

    // load to GPU
    glBindBuffer (GL_UNIFORM_BUFFER, buffers->UBO[UBO_MATRICES]);
    glBufferSubData (GL_UNIFORM_BUFFER, 0, 16 * sizeof (GLfloat), mBuffer->viewMatrix);
    glBufferSubData (GL_UNIFORM_BUFFER, 16 * sizeof (GLfloat), 16 * sizeof (GLfloat), mBuffer->projectionMatrix);
    glBindBuffer (GL_UNIFORM_BUFFER, 0);
}

void GLRenderer::InitializeShaderLibrary ()
{
    GLShaderProgram* p;

    // load standard shaders
    Game::GetResourceManager ().Load<GLShader> ("default_vertex_shader", "shaders/default.vs", ShaderType::VERTEX_SHADER);
    Game::GetResourceManager ().Load<GLShader> ("flatquad_vertex_shader", "shaders/flatquad.vs", ShaderType::VERTEX_SHADER);
    Game::GetResourceManager ().Load<GLShader> ("pbr_forward_fragment_shader", "shaders/pbr_forward.fs", ShaderType::FRAGMENT_SHADER);    
    Game::GetResourceManager ().Load<GLShader> ("pbr_deferred_geometrypass_fragment_shader", "shaders/pbr_deferred_geometrypass.fs", ShaderType::FRAGMENT_SHADER); 
    Game::GetResourceManager ().Load<GLShader> ("pbr_deferred_lightingpass_fragment_shader", "shaders/pbr_deferred_lightingpass.fs", ShaderType::FRAGMENT_SHADER); 
    Game::GetResourceManager ().Load<GLShader> ("blinnphong_forward_fragment_shader", "shaders/blinnphong_forward.fs", ShaderType::FRAGMENT_SHADER);
    Game::GetResourceManager ().Load<GLShader> ("blinnphong_deferred_geometrypass_fragment_shader", "shaders/blinnphong_deferred_geometrypass.fs", ShaderType::FRAGMENT_SHADER); 
    Game::GetResourceManager ().Load<GLShader> ("blinnphong_deferred_lightingpass_fragment_shader", "shaders/blinnphong_deferred_lightingpass.fs", ShaderType::FRAGMENT_SHADER); 
    Game::GetResourceManager ().Load<GLShader> ("flatquad_fragment_shader", "shaders/flatquad.fs", ShaderType::FRAGMENT_SHADER);
    Game::GetResourceManager ().Load<GLShader> ("post_hdr_fragment_shader", "shaders/post_hdr.fs", ShaderType::FRAGMENT_SHADER);
    Game::GetResourceManager ().Load<GLShader> ("post_gamma_fragment_shader", "shaders/post_gamma.fs", ShaderType::FRAGMENT_SHADER);
    Game::GetResourceManager ().Load<GLShader> ("post_fxaa_fragment_shader", "shaders/post_fxaa.fs", ShaderType::FRAGMENT_SHADER);

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

void GLRenderer::InitializeQuadGeometryBuffer ()
{
    SGLGeometryBuffers* buffer = static_cast<SGLGeometryBuffers*>(quadGeometryBuffer);

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

    glGenVertexArrays (1, &buffer->VAO);    
    glBindVertexArray (buffer->VAO);

    glGenBuffers (1, &buffer->VBO[EVBOType::VBO_VERTICES]);
    glGenBuffers (1, &buffer->VBO[EVBOType::VBO_UVS]);

    glBindBuffer (GL_ARRAY_BUFFER, buffer->VBO[EVBOType::VBO_VERTICES]);
    glBufferData (GL_ARRAY_BUFFER, sizeof (quadVertices), &quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer (EVBOType::VBO_VERTICES, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer (GL_ARRAY_BUFFER, buffer->VBO[EVBOType::VBO_UVS]);
    glBufferData (GL_ARRAY_BUFFER, sizeof (quadUV), &quadUV, GL_STATIC_DRAW);
    glVertexAttribPointer (EVBOType::VBO_UVS, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glEnableVertexAttribArray (EVBOType::VBO_VERTICES);
    glEnableVertexAttribArray (EVBOType::VBO_UVS);

    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindVertexArray (0);    

    buffer->vertexCount = 6;

    CheckGLError (MSG_LOCATION);
}

void GLRenderer::DeinitializeQuadGeometryBuffer ()
{
    SGLGeometryBuffers* buffer = static_cast<SGLGeometryBuffers*>(quadGeometryBuffer);

    glDeleteVertexArrays(1, &buffer->VAO);
    glDeleteBuffers(1, &buffer->VBO[EVBOType::VBO_VERTICES]);
}

void GLRenderer::InitializeMatrixUniformBuffers ()
{
    SGLUniformBuffers* buffers = static_cast<SGLUniformBuffers*>(uniformBuffers);

    // create and pre-load uniform buffer for view and projection matrices
    glGenBuffers (1, &buffers->UBO[UBO_MATRICES]);
    glBindBuffer (GL_UNIFORM_BUFFER, buffers->UBO[UBO_MATRICES]);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGLUniformMatrixBuffer), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, BindingPoint::BP_MATRICES, buffers->UBO[UBO_MATRICES]);

    CheckGLError (MSG_LOCATION);
}

GLuint GLRenderer::GetTextureFormat (unsigned int numTextures)
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

void GLRenderer::CheckGLError (const std::string& location)
{
    GLuint errorCode;

    if ((errorCode = glGetError ()) != GL_NO_ERROR)
    {
        LogMessage (location, EXIT_FAILURE) << "glError:" << std::hex << std::showbase << errorCode;
    }
}

void GLRenderer::Update (PointLight& pointLight)
{
    SGLUniformPointLightBuffer* uniformBuffer = static_cast<SGLUniformPointLightBuffer*>(uniformPointLightBuffer);
    SGLUniformBuffers* buffers = static_cast<SGLUniformBuffers*>(uniformBuffers);

    unsigned int objectHandle = pointLight.GetHandle ();
    unsigned int lightId;
    unsigned int uniformBufferOffset;

    // check if light is already in collection
    auto find = pointLights.find (objectHandle);

    if (find == pointLights.end ())
    {
        lightId = uniformBuffer->pointLightCount++;
        pointLights.insert ({ objectHandle, lightId });
    }
    else
    {
        // existing light modified
        lightId = pointLights[objectHandle];
    }

    // copy position
    Vector4f lightPosition = pointLight.GetPosition ();
    uniformBuffer->pointLights[lightId].lightPosition[0] = lightPosition[0];
    uniformBuffer->pointLights[lightId].lightPosition[1] = lightPosition[1];
    uniformBuffer->pointLights[lightId].lightPosition[2] = lightPosition[2];

    // copy attenuation factors
    uniformBuffer->pointLights[lightId].attenuationConst = pointLight.GetConstantAttenuationFactor ();
    uniformBuffer->pointLights[lightId].attenuationLinear = pointLight.GetLinearAttenuationFactor ();
    uniformBuffer->pointLights[lightId].attenuationQuadratic = pointLight.GetQuadraticAttenuationFactor ();

    // copy color
    uniformBuffer->pointLights[lightId].lightColor[0] = pointLight.GetColor ()[0];
    uniformBuffer->pointLights[lightId].lightColor[1] = pointLight.GetColor ()[1];
    uniformBuffer->pointLights[lightId].lightColor[2] = pointLight.GetColor ()[2];

    // copy to GPU memory
    glBindBuffer (GL_UNIFORM_BUFFER, buffers->UBO[UBO_POINTLIGHTS]);

    // load light counts
    glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (uniformBuffer->pointLightCount), &uniformBuffer->pointLightCount);

    // load uniform buffer for a light at given index
    uniformBufferOffset = sizeof (uniformBuffer->pointLightCount) + 3 * sizeof (GLint) + lightId * sizeof (SGLPointLightStruct);
    glBufferSubData (GL_UNIFORM_BUFFER, uniformBufferOffset, sizeof (SGLPointLightStruct), &uniformBuffer->pointLights[lightId]);

    glBindBuffer (GL_UNIFORM_BUFFER, 0);

}

void GLRenderer::Update (DirectionalLight& directionalLight)
{
    SGLUniformDirectionalLightBuffer* uniformBuffer = static_cast<SGLUniformDirectionalLightBuffer*>(uniformDirectionalLightBuffer);
    SGLUniformBuffers* buffers = static_cast<SGLUniformBuffers*>(uniformBuffers);

    unsigned int objectHandle = directionalLight.GetHandle ();
    unsigned int lightId;
    unsigned int uniformBufferOffset;

    // check if light is already in collection
    auto find = directionalLights.find (objectHandle);

    if (find == directionalLights.end ())
    {
        lightId = uniformBuffer->directionalLightCount++;
        directionalLights.insert ({ objectHandle, lightId });
    }
    else
    {
        // existing light modified
        lightId = directionalLights[objectHandle];
    }

    // copy direction
    Vector3f lightDirection = directionalLight.GetForward ();
    uniformBuffer->directionalLights[lightId].lightDirection[0] = lightDirection[0];
    uniformBuffer->directionalLights[lightId].lightDirection[1] = lightDirection[1];
    uniformBuffer->directionalLights[lightId].lightDirection[2] = lightDirection[2];

    // copy color
    uniformBuffer->directionalLights[lightId].lightColor[0] = directionalLight.GetColor ()[0];
    uniformBuffer->directionalLights[lightId].lightColor[1] = directionalLight.GetColor ()[1];
    uniformBuffer->directionalLights[lightId].lightColor[2] = directionalLight.GetColor ()[2];

    // copy to GPU memory
    glBindBuffer (GL_UNIFORM_BUFFER, buffers->UBO[UBO_DIRECTIONALLIGHTS]);

    // load light counts
    glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (uniformBuffer->directionalLightCount), &uniformBuffer->directionalLightCount);

    // load uniform buffer for a light at given index
    uniformBufferOffset = sizeof (uniformBuffer->directionalLightCount) + 3 * sizeof (GLint) + lightId * sizeof (SGLDirectionalLightStruct);
    glBufferSubData (GL_UNIFORM_BUFFER, uniformBufferOffset, sizeof (SGLDirectionalLightStruct), &uniformBuffer->directionalLights[lightId]);

    glBindBuffer (GL_UNIFORM_BUFFER, 0);
}

void GLRenderer::Update (SpotLight& spotLight)
{
    SGLUniformSpotLightBuffer* uniformBuffer = static_cast<SGLUniformSpotLightBuffer*>(uniformSpotLightBuffer);
    SGLUniformBuffers* buffers = static_cast<SGLUniformBuffers*>(uniformBuffers);

    unsigned int objectHandle = spotLight.GetHandle ();
    unsigned int lightId;
    unsigned int uniformBufferOffset;

    // check if light is already in collection
    auto find = spotLights.find (objectHandle);

    if (find == spotLights.end ())
    {
        lightId = uniformBuffer->spotLightCount++;
        spotLights.insert ({ objectHandle, lightId });
    }
    else
    {
        // existing light modified
        lightId = spotLights[objectHandle];
    }

    // copy position
    Vector4f lightPosition = spotLight.GetPosition ();
    uniformBuffer->spotLights[lightId].lightPosition[0] = lightPosition[0];
    uniformBuffer->spotLights[lightId].lightPosition[1] = lightPosition[1];
    uniformBuffer->spotLights[lightId].lightPosition[2] = lightPosition[2];

    // copy direction
    Vector3f lightDirection = spotLight.GetForward ();
    uniformBuffer->spotLights[lightId].lightDirection[0] = lightDirection[0];
    uniformBuffer->spotLights[lightId].lightDirection[1] = lightDirection[1];
    uniformBuffer->spotLights[lightId].lightDirection[2] = lightDirection[2];
    
    // copy attenuation factors
    uniformBuffer->spotLights[lightId].attenuationConst = spotLight.GetConstantAttenuationFactor ();
    uniformBuffer->spotLights[lightId].attenuationLinear = spotLight.GetLinearAttenuationFactor ();
    uniformBuffer->spotLights[lightId].attenuationQuadratic = spotLight.GetQuadraticAttenuationFactor ();

    // copy cutoff angles
    uniformBuffer->spotLights[lightId].innerCutoffCosine = std::cos (Mathf::Deg2Rad (spotLight.GetInnerCutoff ()));
    uniformBuffer->spotLights[lightId].outerCutoffCosine = std::cos (Mathf::Deg2Rad (spotLight.GetOuterCutoff ()));

    // copy color
    uniformBuffer->spotLights[lightId].lightColor[0] = spotLight.GetColor ()[0];
    uniformBuffer->spotLights[lightId].lightColor[1] = spotLight.GetColor ()[1];
    uniformBuffer->spotLights[lightId].lightColor[2] = spotLight.GetColor ()[2];

    // copy to GPU memory
    glBindBuffer (GL_UNIFORM_BUFFER, buffers->UBO[UBO_SPOTLIGHTS]);

    // load light counts
    glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (uniformBuffer->spotLightCount), &uniformBuffer->spotLightCount);

    // load uniform buffer for a light at given index
    uniformBufferOffset = sizeof (uniformBuffer->spotLightCount) + 3 * sizeof (GLint) + lightId * sizeof (SGLSpotLightStruct);
    glBufferSubData (GL_UNIFORM_BUFFER, uniformBufferOffset, sizeof (SGLSpotLightStruct), &uniformBuffer->spotLights[lightId]);

    glBindBuffer (GL_UNIFORM_BUFFER, 0);
}

void GLRenderer::InitializeLightUniformBuffers ()
{
    SGLUniformBuffers* buffers = static_cast<SGLUniformBuffers*>(uniformBuffers);
    SGLUniformPointLightBuffer* pointLightUniformBuf = static_cast<SGLUniformPointLightBuffer*>(uniformPointLightBuffer);
    SGLUniformSpotLightBuffer* spotLightUniformBuf = static_cast<SGLUniformSpotLightBuffer*>(uniformSpotLightBuffer);
    SGLUniformDirectionalLightBuffer* directionalLightUniformBuf = static_cast<SGLUniformDirectionalLightBuffer*>(uniformDirectionalLightBuffer);

    pointLightUniformBuf->pointLightCount = 0;
    spotLightUniformBuf->spotLightCount = 0;
    directionalLightUniformBuf->directionalLightCount = 0;

    // create uniform buffer for point lights
    glGenBuffers (1, &buffers->UBO[UBO_POINTLIGHTS]);
    glBindBuffer (GL_UNIFORM_BUFFER, buffers->UBO[UBO_POINTLIGHTS]);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGLUniformPointLightBuffer), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, BindingPoint::BP_POINTLIGHTS, buffers->UBO[UBO_POINTLIGHTS]);

    // create uniform buffer for directional lights
    glGenBuffers (1, &buffers->UBO[UBO_DIRECTIONALLIGHTS]);
    glBindBuffer (GL_UNIFORM_BUFFER, buffers->UBO[UBO_DIRECTIONALLIGHTS]);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGLUniformDirectionalLightBuffer), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, BindingPoint::BP_DIRECTIONALLIGHTS, buffers->UBO[UBO_DIRECTIONALLIGHTS]);

    // create uniform buffer for spot lights
    glGenBuffers (1, &buffers->UBO[UBO_SPOTLIGHTS]);
    glBindBuffer (GL_UNIFORM_BUFFER, buffers->UBO[UBO_SPOTLIGHTS]);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGLUniformSpotLightBuffer), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, BindingPoint::BP_SPOTLIGHTS, buffers->UBO[UBO_SPOTLIGHTS]);

    // scan objects vector for lights and populate light buffers
    for (auto gameObject : gameScene->GetGameObjectManager ())
    {
        if (std::dynamic_pointer_cast<Light> (gameObject) != nullptr)
        {
            gameObject->OnUpdate (*this);
        }
    }

}

void GLRenderer::UpdateLightBufferRecursive (unsigned int objectHandle)
{
    GameObject* light = &gameScene->GetGameObjectManager ().GetByHandle<GameObject> (objectHandle);

    if (dynamic_cast<Light*>(light) != nullptr)
    {
        light->OnUpdate (*this);
    }

    for (auto& childObject : light->GetChildObjects ())
    {
        UpdateLightBufferRecursive (childObject->GetHandle ());
    }

}
