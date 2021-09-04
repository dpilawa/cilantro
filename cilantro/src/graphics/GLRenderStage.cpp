#include "graphics/GLRenderStage.h"
#include "system/LogMessage.h"
#include "system/EngineContext.h"

GLRenderStage::GLRenderStage () : RenderStage ()
{
}

GLRenderStage::~GLRenderStage ()
{
}

RenderStage& GLRenderStage::SetMultisampleFramebufferEnabled (bool value)
{
    if (framebuffer != nullptr)
    {
        framebuffer->Deinitialize ();
        multisampleFramebufferEnabled = value;
        framebuffer->Initialize ();
    }

    return RenderStage::SetMultisampleFramebufferEnabled (value);
}

void GLRenderStage::Initialize ()
{
    // initialize framebuffers
    if (multisampleFramebufferEnabled)
    {
#if (CILANTRO_GL_VERSION <= 140)
        framebuffer = new GLFramebuffer (EngineContext::GetRenderer ().GetFramebuffer ()->GetWidth (), EngineContext::GetRenderer ().GetFramebuffer ()->GetHeight (), 0, 1);
#else
        framebuffer = new GLMultisampleFramebuffer (EngineContext::GetRenderer ().GetFramebuffer ()->GetWidth (), EngineContext::GetRenderer ().GetFramebuffer ()->GetHeight (), 0, 1);
#endif
    }
    else
    {
        framebuffer = new GLFramebuffer (EngineContext::GetRenderer ().GetFramebuffer ()->GetWidth (), EngineContext::GetRenderer ().GetFramebuffer ()->GetHeight (), 0, 1);
    }
    
    framebuffer->Initialize ();

    // set up VBO and VAO
    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays (1, &VAO);
    glGenBuffers (1, &VBO);
    glBindVertexArray (VAO);
    glBindBuffer (GL_ARRAY_BUFFER, VBO);
    glBufferData (GL_ARRAY_BUFFER, sizeof (quadVertices), &quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);    
    glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof (float)));
    glEnableVertexAttribArray (0);
    glEnableVertexAttribArray (1);
    glBindVertexArray (0);    
    glBindBuffer (GL_ARRAY_BUFFER, 0);

    LogMessage (MSG_LOCATION) << "GLRenderStage initialized" << this->GetName ();
}

void GLRenderStage::Deinitialize ()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    framebuffer->Deinitialize ();
}

void GLRenderStage::OnFrame ()
{
    GLuint glStencilFunction;
    GLFramebuffer* inputFramebuffer = dynamic_cast<GLFramebuffer*>(EngineContext::GetRenderer ().GetPipelineFramebuffer (pipelineFramebufferInputLink));
    GLFramebuffer* inputFramebufferRenderbuffer = dynamic_cast<GLFramebuffer*>(EngineContext::GetRenderer ().GetPipelineFramebuffer (pipelineRenderbufferLink));
    GLFramebuffer* outputFramebuffer = dynamic_cast<GLFramebuffer*>(EngineContext::GetRenderer ().GetPipelineFramebuffer (pipelineFramebufferOutputLink));
  
    // attach input renderbuffer's stencil to output
    glBindFramebuffer (GL_FRAMEBUFFER, outputFramebuffer->GetFramebufferGLId ());
    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, inputFramebufferRenderbuffer->GetFramebufferRenderbufferGLId ());

    // optionally clear
    if (clearOnFrameEnabled)
    {
        glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
        glClear (GL_COLOR_BUFFER_BIT);
    }
    
    // disable depth test
    glDisable (GL_DEPTH_TEST);
    if (depthTestEnabled)
    {
        glEnable (GL_DEPTH_TEST);
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

    // bind previous (input) framebuffer textures and draw
    shaderProgram->Use ();

    glBindVertexArray (VAO);
    for (int i = 0; i < inputFramebuffer->GetTextureCount (); i++)
    {
        glActiveTexture (GL_TEXTURE0 + i);
        glBindTexture (GL_TEXTURE_2D, inputFramebuffer->GetFramebufferTextureGLId (i));
    }
    
    glViewport (0, 0, outputFramebuffer->GetWidth (), outputFramebuffer->GetHeight ());
    glDrawArrays (GL_TRIANGLES, 0, 6);
    glBindTexture (GL_TEXTURE_2D, 0);
    glBindVertexArray (0);
    glBindFramebuffer (GL_FRAMEBUFFER, 0);
}

RenderStage& GLRenderStage::SetRenderStageParameterFloat (const std::string& parameterName, float parameterValue)
{
    GLuint location = GetUniformLocation (parameterName);
    glUniform1f (location, parameterValue);

    return *this;
}

RenderStage& GLRenderStage::SetRenderStageParameterVector2f (const std::string& parameterName, const Vector2f& parameterValue)
{
    GLuint location = GetUniformLocation (parameterName);
    glUniform2fv (location, 1, &parameterValue[0]);
    
    return *this;
}

RenderStage& GLRenderStage::SetRenderStageParameterVector3f (const std::string& parameterName, const Vector3f& parameterValue)
{    
    GLuint location = GetUniformLocation (parameterName);
    glUniform3fv (location, 1, &parameterValue[0]);
    
    return *this;
}

RenderStage& GLRenderStage::SetRenderStageParameterVector4f (const std::string& parameterName, const Vector4f& parameterValue)
{
    GLuint location = GetUniformLocation (parameterName);
    glUniform4fv (location, 1, &parameterValue[0]);
    
    return *this;
}

GLuint GLRenderStage::GetUniformLocation (const std::string& parameterName)
{
    GLShaderProgram* glShaderProgam = dynamic_cast<GLShaderProgram*> (shaderProgram);

    glShaderProgam->Use ();
    GLuint paramUniformLocation = glGetUniformLocation (glShaderProgam->GetProgramId (), parameterName.c_str ());

    if (paramUniformLocation == GL_INVALID_INDEX)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Uniform not found in stage shader:" << parameterName;
    }

    return paramUniformLocation;
}

