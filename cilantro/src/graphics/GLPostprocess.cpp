#include "graphics/GLPostprocess.h"
#include "util/LogMessage.h"

GLPostprocess::GLPostprocess (GLRenderer* renderer, GLShaderProgram* shaderProgram) : Postprocess (renderer, shaderProgram), GLFramebuffer (renderer->GetWidth (), renderer->GetHeight ())
{
    this->Initialize ();
    LogMessage (__func__) << "GLPostprocess started for program" << shaderProgram->GetProgramId ();
}

GLPostprocess::~GLPostprocess ()
{
    
}

void GLPostprocess::OnFrame ()
{
    // draw quad on screen
    GLRenderer* glRenderer = dynamic_cast<GLRenderer*>(renderer);

    glBindFramebuffer (GL_FRAMEBUFFER, this->GetFramebuffer ());
    glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable (GL_DEPTH_TEST);
    shaderProgram->Use ();
    glBindVertexArray (VAO);
    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D, glRenderer->GetRendererFramebufferTexture ()); 
    glViewport (0, 0, renderer->GetWidth (), renderer->GetHeight ());
    glDrawArrays (GL_TRIANGLES, 0, 6);
}

void GLPostprocess::SetPostprocessParameterFloat (std::string parameterName, float parameterValue)
{
    GLShaderProgram* glShaderProgam = dynamic_cast<GLShaderProgram*> (shaderProgram);

    glShaderProgam->Use ();
    GLuint paramUniformLocation = glGetUniformLocation (glShaderProgam->GetProgramId (), parameterName.c_str ());

    if (paramUniformLocation == GL_INVALID_INDEX)
    {
        LogMessage (__func__, EXIT_FAILURE) << "Uniform not found in postprocessor shader:" << parameterName;
    }

    glUniform1f (paramUniformLocation, parameterValue);
}

void GLPostprocess::Initialize ()
{
    // set up VBO and VAO
     float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof (quadVertices), &quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof (float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}