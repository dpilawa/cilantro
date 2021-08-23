#include "system/EngineContext.h"
#include "graphics/GLForwardRenderer.h"
#include "scene/MeshObject.h"

GLForwardRenderer::GLForwardRenderer (unsigned int width, unsigned int height) : GLRenderer (width, height)
{
#if (CILANTRO_GL_VERSION <= 140)
    framebuffer = new GLFramebuffer (width, height, 0, 1);
#else
    framebuffer = new GLMultisampleFramebuffer (width, height, 0, 1);
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
    framebuffer->BindFramebuffer ();

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

    // base class function
    postprocessStage = 0;
    Renderer::RenderFrame ();

    // unbind framebuffer
    framebuffer->UnbindFramebuffer ();

    // check for errors
    CheckGLError (MSG_LOCATION);
}

Framebuffer* GLForwardRenderer::GetCurrentFramebuffer () const
{
    if (postprocessStage == 0) 
    {
        return GetFramebuffer ();
    }
    else 
    {
        return (*(postprocesses.begin() + (postprocessStage - 1))).get()->GetFramebuffer ();
    }
}

ShaderProgram& GLForwardRenderer::GetMeshObjectGeometryShaderProgram (const MeshObject& meshObject) 
{
    return meshObject.GetMaterial ().GetForwardShaderProgram ();
}

ShaderProgram& GLForwardRenderer::GetMeshObjectLightingShaderProgram (const MeshObject& meshObject) 
{
    return meshObject.GetMaterial ().GetForwardShaderProgram ();
}