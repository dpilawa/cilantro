#include "cilantroengine.h"
#include "graphics/GLShader.h"
#include "graphics/GLShaderProgram.h"
#include "system/LogMessage.h"

#include <iostream>

GLShaderProgram::GLShaderProgram () : ShaderProgram ()
{
    shaderProgramId = glCreateProgram ();
}


GLShaderProgram::~GLShaderProgram ()
{
}

GLuint GLShaderProgram::GetProgramId () const
{
    return shaderProgramId;
}


void GLShaderProgram::LinkShader (const Shader& shader)
{
    GLint success;
    char errorLog[512];

    const GLShader* glShader = static_cast<const GLShader*> (&shader);

    glAttachShader (shaderProgramId, glShader->GetShaderId ());
    glLinkProgram (shaderProgramId);

    glGetProgramiv (shaderProgramId, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog (shaderProgramId, 512, nullptr, errorLog);
        glDeleteProgram (shaderProgramId);
        LogMessage () << errorLog;
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unable to link shader" << glShader->GetShaderId () << " to program" << shaderProgramId;
    }
}

void GLShaderProgram::Use ()
{
    glUseProgram (shaderProgramId);
}
