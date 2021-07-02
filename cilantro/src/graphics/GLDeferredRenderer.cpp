#include "system/EngineContext.h"
#include "graphics/GLDeferredRenderer.h"
#include "scene/MeshObject.h"

GLDeferredRenderer::GLDeferredRenderer (unsigned int width, unsigned int height) : GLRenderer (width, height)
{
#if (CILANTRO_GL_VERSION <= 140)
    framebuffer = new GLFramebuffer (width, height, 1, 0);
#else
    framebuffer = new GLMultisampleFramebuffer (width, height, 1, 0);
#endif

    gBuffer = new GLFramebuffer (width, height, 3, 1);
}

GLDeferredRenderer::~GLDeferredRenderer ()
{

}

void GLDeferredRenderer::Initialize ()
{
    // initialize framebuffer
    framebuffer->Initialize ();
    GLRenderer::Initialize ();
}

void GLDeferredRenderer::RenderFrame ()
{
    // bind g-buffer
    static_cast<GLFramebuffer*>(gBuffer)->BindFramebuffer ();

    // clear frame and depth buffers
    glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // set viewport
    glViewport (0, 0, this->GetFramebuffer ()->GetWidth (), this->GetFramebuffer ()->GetHeight ());

    // enable stencil buffer testing
    glEnable (GL_STENCIL_TEST);

    // draw all objects in scene
    for (auto gameObject : EngineContext::GetGameScene ().GetGameObjectManager ())
    {
        // overwrite stencil value with material Id
        if (MeshObject* meshObject = dynamic_cast<MeshObject*>(gameObject.get ()))
        {
            glStencilFunc (GL_ALWAYS, meshObject->GetMaterial ().GetHandle (), 0xff);
            glStencilOp (GL_KEEP, GL_KEEP, GL_REPLACE);
        }
        else
        {
            glStencilFunc (GL_NEVER, 0, 0xff);
            glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);          
        }

        // draw to g-buffer
        gameObject->OnDraw (*this);
    }

    // base class function
    Renderer::RenderFrame ();

    // unbind framebuffer
    static_cast<GLFramebuffer*>(framebuffer)->UnbindFramebuffer ();

    // check for errors
    CheckGLError (MSG_LOCATION);    
}

GLShaderProgram& GLDeferredRenderer::GetMeshObjectShaderProgram (const MeshObject& meshObject) 
{
    GLShaderProgram& shaderProgram = GetShaderProgramManager ().GetByName<GLShaderProgram> (meshObject.GetMaterial ().GetDeferredShaderProgramName ());

    return shaderProgram;
}