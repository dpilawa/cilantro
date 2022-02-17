#include "graphics/GLRenderer.h"
#include "glad/glad.h"
#include "system/Game.h"
#include "graphics/GLShader.h"
#include "graphics/GLShaderProgram.h"
#include "graphics/GLGeometryBuffer.h"
#include "graphics/GLFramebuffer.h"
#if (CILANTRO_GL_VERSION > 140)
#include "graphics/GLMultisampleFramebuffer.h"
#endif

GLRenderer::GLRenderer (GameScene* gameScene, unsigned int width, unsigned int height) : Renderer (gameScene, width, height)
{
    sceneGeometryBuffer = new GLGeometryBuffer ();
    quadGeometryBuffer = new GLGeometryBuffer ();
}

GLRenderer::~GLRenderer ()
{
    delete sceneGeometryBuffer;
    delete quadGeometryBuffer;
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
}

void GLRenderer::RenderGeometryBuffer (GeometryBuffer* buffer)
{
    GLGeometryBuffer* glBuffer = static_cast<GLGeometryBuffer*>(buffer);

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

void GLRenderer::ClearColorBuffer (Vector4f& rgba)
{
    glClearColor (rgba[0], rgba[1], rgba[2], rgba[3]);
    glClear (GL_COLOR_BUFFER_BIT);
}

void GLRenderer::ClearDepthBuffer ()
{
    glClear (GL_DEPTH_BUFFER_BIT);
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

}

void GLRenderer::InitializeQuadGeometryBuffer ()
{
    GLGeometryBuffer* buffer = static_cast<GLGeometryBuffer*>(quadGeometryBuffer);

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

    glGenBuffers (1, &buffer->VBO[VBOType::VBO_VERTICES]);
    glGenBuffers (1, &buffer->VBO[VBOType::VBO_UVS]);

    glBindBuffer (GL_ARRAY_BUFFER, buffer->VBO[VBOType::VBO_VERTICES]);
    glBufferData (GL_ARRAY_BUFFER, sizeof (quadVertices), &quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer (VBOType::VBO_VERTICES, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer (GL_ARRAY_BUFFER, buffer->VBO[VBOType::VBO_UVS]);
    glBufferData (GL_ARRAY_BUFFER, sizeof (quadUV), &quadUV, GL_STATIC_DRAW);
    glVertexAttribPointer (VBOType::VBO_UVS, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glEnableVertexAttribArray (VBOType::VBO_VERTICES);
    glEnableVertexAttribArray (VBOType::VBO_UVS);

    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindVertexArray (0);    

    buffer->vertexCount = 6;
}

void GLRenderer::DeinitializeQuadGeometryBuffer ()
{
    GLGeometryBuffer* buffer = static_cast<GLGeometryBuffer*>(quadGeometryBuffer);

    glDeleteVertexArrays(1, &buffer->VAO);
    glDeleteBuffers(1, &buffer->VBO[VBOType::VBO_VERTICES]);
}