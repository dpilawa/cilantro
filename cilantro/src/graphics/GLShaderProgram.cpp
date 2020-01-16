#include "cilantroengine.h"
#include "graphics/GLShader.h"
#include "graphics/GLShaderProgram.h"
#include "util/LogMessage.h"

#include <iostream>

GLShaderProgram::GLShaderProgram () : ShaderProgram ()
{
    shaderProgramId = glCreateProgram ();
}


GLShaderProgram::~GLShaderProgram ()
{
}

GLuint GLShaderProgram::GetProgramId ()
{
    return shaderProgramId;
}


void GLShaderProgram::LinkShader (Shader& shader)
{
    GLint success;
    char errorLog[512];

    GLShader* glShader = static_cast<GLShader*> (&shader);

    glAttachShader (shaderProgramId, glShader->GetShaderId ());
    glLinkProgram (shaderProgramId);

    glGetProgramiv (shaderProgramId, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog (shaderProgramId, 512, nullptr, errorLog);
        glDeleteProgram (shaderProgramId);
        LogMessage () << errorLog;
        LogMessage (__func__, EXIT_FAILURE) << "Unable to link shader" << glShader->GetShaderId () << " to program" << shaderProgramId;
    }
}

void GLShaderProgram::Use ()
{
    glUseProgram (shaderProgramId);
}
