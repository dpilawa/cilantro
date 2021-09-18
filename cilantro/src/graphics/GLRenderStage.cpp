#include "graphics/GLRenderStage.h"
#include "system/LogMessage.h"
#include "system/EngineContext.h"

GLRenderStage::GLRenderStage () : RenderStage ()
{
}

GLRenderStage::~GLRenderStage ()
{
}

RenderStage& GLRenderStage::SetMultisampleEnabled (bool value)
{
    if (framebuffer != nullptr)
    {
        framebuffer->Deinitialize ();
        delete framebuffer;
    }    
    
    multisampleEnabled = value;

    if (multisampleEnabled)
    {
#if (CILANTRO_GL_VERSION <= 140)
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "OpenGL 3.2 required for multisample framebuffers";
#else
        framebuffer = new GLMultisampleFramebuffer (EngineContext::GetRenderer ().GetWidth (), EngineContext::GetRenderer ().GetHeight (), 0, 1);
#endif  
    }
    else 
    {
        framebuffer = new GLFramebuffer (EngineContext::GetRenderer ().GetWidth (), EngineContext::GetRenderer ().GetHeight (), 0, 1);
    }

    framebuffer->Initialize ();

    return RenderStage::SetMultisampleEnabled (value);
}

void GLRenderStage::Initialize ()
{
    LogMessage (MSG_LOCATION) << "GLRenderStage initialized" << this->GetName ();
}

void GLRenderStage::Deinitialize ()
{
    if (framebuffer != nullptr)
    {
        framebuffer->Deinitialize ();
    }
}

void GLRenderStage::OnFrame ()
{
    GLuint glStencilFunction;
    Framebuffer* inputFramebuffer = EngineContext::GetRenderer ().GetPipelineFramebuffer (pipelineFramebufferInputLink);
    Framebuffer* inputFramebufferRenderbuffer = EngineContext::GetRenderer ().GetPipelineFramebuffer (pipelineRenderbufferLink);
    Framebuffer* outputFramebuffer = EngineContext::GetRenderer ().GetPipelineFramebuffer (pipelineFramebufferOutputLink);
  
    // attach input renderbuffer's stencil to output
    if (outputFramebuffer != nullptr)
    {
        glBindFramebuffer (GL_FRAMEBUFFER, dynamic_cast<GLFramebuffer*>(outputFramebuffer)->GetDrawFramebufferGLId ());
    }
    else
    {
        // bind default framebuffer
        glBindFramebuffer (GL_FRAMEBUFFER, (GLint)0);
    }
    
    if (inputFramebufferRenderbuffer)
    {
        glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, dynamic_cast<GLFramebuffer*>(inputFramebufferRenderbuffer)->GetDrawFramebufferRenderbufferGLId ());
    }

    // optionally clear
    if (clearOnFrameEnabled)
    {
        glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
        glClear (GL_COLOR_BUFFER_BIT);
    }
    
    // optionally enable depth test
    glDisable (GL_DEPTH_TEST);
    if (depthTestEnabled)
    {
        glEnable (GL_DEPTH_TEST);
        glClear (GL_DEPTH_BUFFER_BIT);
    }

    // optionally enable face culling
    glDisable (GL_CULL_FACE);
    if (faceCullingEnabled)
    {
        glFrontFace (GL_CCW);
        glEnable (GL_CULL_FACE);
    }

    // optionally enable multisampling
    glDisable (GL_MULTISAMPLE);
    if (multisampleEnabled)
    {
        glEnable (GL_MULTISAMPLE);
    }

    // optionally enable stencil test
    glDisable (GL_STENCIL_TEST);
    if (stencilTestEnabled)
    {
        glEnable (GL_STENCIL_TEST);
        glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
        switch (stencilTestFunction)   
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
        glStencilFunc (glStencilFunction, stencilTestValue, 0xff);
    }

}

void GLRenderStage::InitializeFramebuffer (unsigned int rgbTextures, unsigned int rgbaTextures)
{
    // initialize framebuffers
    if (framebufferEnabled)
    {
        if (multisampleEnabled)
        {
#if (CILANTRO_GL_VERSION <= 140)
            LogMessage (MSG_LOCATION, EXIT_FAILURE) << "OpenGL 3.2 required for multisample framebuffers";
#else
            framebuffer = new GLMultisampleFramebuffer (EngineContext::GetRenderer ().GetWidth (), EngineContext::GetRenderer ().GetHeight (), rgbTextures, rgbaTextures);
#endif
        }
        else
        {
            framebuffer = new GLFramebuffer (EngineContext::GetRenderer ().GetWidth (), EngineContext::GetRenderer ().GetHeight (), rgbTextures, rgbaTextures);
        }
        
        framebuffer->Initialize ();
    }
}
