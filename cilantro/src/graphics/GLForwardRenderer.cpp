#include "system/EngineContext.h"
#include "graphics/GLForwardRenderer.h"
#include "scene/MeshObject.h"

GLForwardRenderer::GLForwardRenderer (unsigned int width, unsigned int height) : GLRenderer (width, height)
{
#if (CILANTRO_GL_VERSION <= 140)
    framebuffer = new GLFramebuffer (width, height, 1, 0);
#else
    framebuffer = new GLMultisampleFramebuffer (width, height, 1, 0);
#endif
}

GLForwardRenderer::~GLForwardRenderer ()
{

}

void GLForwardRenderer::Initialize ()
{
    // initialize framebuffer
    framebuffer->Initialize ();
    GLRenderer::Initialize ();
}

void GLForwardRenderer::RenderFrame ()
{
    // bind framebuffer
    static_cast<GLFramebuffer*>(framebuffer)->BindFramebuffer ();

    // clear frame and depth buffers
    glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // enable depth test
    glEnable (GL_DEPTH_TEST);

    // load uniform buffers
    LoadMatrixUniformBuffers ();

    // set viewport
    glViewport (0, 0, this->GetFramebuffer ()->GetWidth (), this->GetFramebuffer ()->GetHeight ());

    // draw all objects in scene
    for (auto gameObject : EngineContext::GetGameScene ().GetGameObjectManager ())
    {
        gameObject->OnDraw (*this);
    }

#if (CILANTRO_GL_VERSION > 140)
    // blit framebuffer
    static_cast<GLMultisampleFramebuffer*>(framebuffer)->BlitFramebuffer ();
#endif

    // base class functions
    Renderer::RenderFrame ();

    // check for errors
    CheckGLError (MSG_LOCATION);
}

GLShaderProgram& GLForwardRenderer::GetMeshObjectShaderProgram (const MeshObject& meshObject) 
{
    GLShaderProgram& shaderProgram = GetShaderProgramManager ().GetByName<GLShaderProgram> (meshObject.GetMaterial ().GetForwardShaderProgramName ());

    return shaderProgram;
}
