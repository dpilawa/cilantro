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

void GLShaderProgram::BindUniformBlock (const std::string& blockName, BindingPoint bp)
{
    GLuint uniformBlockIndex = glGetUniformBlockIndex (shaderProgramId, blockName.c_str ());

    if (uniformBlockIndex != GL_INVALID_INDEX)
    {
        glUniformBlockBinding (shaderProgramId, uniformBlockIndex, bp);
    }	
    else
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Shader" << this->GetName () << "has no uniform" << blockName;
    }
    
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
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unable to link shader" << glShader->GetShaderId () << "to program" << shaderProgramId;
    }
}

void GLShaderProgram::Use ()
{
    glUseProgram (shaderProgramId);
}
