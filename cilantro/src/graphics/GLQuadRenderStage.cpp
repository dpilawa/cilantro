#include "graphics/GLQuadRenderStage.h"
#include "graphics/GLShaderProgram.h"
#include "system/LogMessage.h"
#include "system/Game.h"

GLQuadRenderStage::GLQuadRenderStage () : GLRenderStage (), QuadRenderStage ()
{
}

GLQuadRenderStage::~GLQuadRenderStage ()
{
}

void GLQuadRenderStage::Initialize ()
{
    GLRenderStage::Initialize ();
    InitializeFramebuffer ();

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

    LogMessage (MSG_LOCATION) << "GLQuadRenderStage initialized" << this->GetName ();

}

void GLQuadRenderStage::Deinitialize ()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    GLRenderStage::Deinitialize ();
}

void GLQuadRenderStage::OnFrame ()
{
    GLFramebuffer* inputFramebuffer = dynamic_cast<GLFramebuffer*>(Game::GetRenderer ().GetPipelineFramebuffer (pipelineFramebufferInputLink));

    GLRenderStage::OnFrame ();
 
    // bind previous (input) framebuffer textures and draw    

    shaderProgram->Use ();
    
    for (int i = 0; i < inputFramebuffer->GetTextureCount (); i++)
    {
        glActiveTexture (GL_TEXTURE0 + i);
        glBindTexture (GL_TEXTURE_2D, inputFramebuffer->GetReadFramebufferTextureGLId (i));
    }

    glBindVertexArray (VAO);    
    glViewport (0, 0, Game::GetRenderer ().GetWidth(), Game::GetRenderer ().GetHeight ());
    glDrawArrays (GL_TRIANGLES, 0, 6);
    glBindTexture (GL_TEXTURE_2D, 0);
    glBindVertexArray (0);
    glBindFramebuffer (GL_FRAMEBUFFER, 0);

 #if (CILANTRO_GL_VERSION > 140)
    // blit framebuffer
    if (multisampleEnabled)
    {
        static_cast<GLMultisampleFramebuffer*>(framebuffer)->BlitFramebuffer ();
    }
#endif

}

QuadRenderStage& GLQuadRenderStage::SetRenderStageParameterFloat (const std::string& parameterName, float parameterValue)
{
    GLuint location = GetUniformLocation (parameterName);
    glUniform1f (location, parameterValue);

    return *this;
}

QuadRenderStage& GLQuadRenderStage::SetRenderStageParameterVector2f (const std::string& parameterName, const Vector2f& parameterValue)
{
    GLuint location = GetUniformLocation (parameterName);
    glUniform2fv (location, 1, &parameterValue[0]);
    
    return *this;
}

QuadRenderStage& GLQuadRenderStage::SetRenderStageParameterVector3f (const std::string& parameterName, const Vector3f& parameterValue)
{    
    GLuint location = GetUniformLocation (parameterName);
    glUniform3fv (location, 1, &parameterValue[0]);
    
    return *this;
}

QuadRenderStage& GLQuadRenderStage::SetRenderStageParameterVector4f (const std::string& parameterName, const Vector4f& parameterValue)
{
    GLuint location = GetUniformLocation (parameterName);
    glUniform4fv (location, 1, &parameterValue[0]);
    
    return *this;
}

void GLQuadRenderStage::InitializeFramebuffer ()
{
    GLRenderStage::InitializeFramebuffer (0, 1);
}

GLuint GLQuadRenderStage::GetUniformLocation (const std::string& parameterName)
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

