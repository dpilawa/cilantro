#include "graphics/GLShaderProgram.h"
#include "graphics/GLShader.h"
#include "system/LogMessage.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include "math/Vector4f.h"

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

GLuint GLShaderProgram::GetUniformLocationId (const std::string& uniformName) const
{
    this->Use ();
    GLuint paramUniformLocation = glGetUniformLocation (shaderProgramId, uniformName.c_str ());

    if (paramUniformLocation == GL_INVALID_INDEX)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Uniform not found in stage shader:" << uniformName;
    }

    return paramUniformLocation;
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

ShaderProgram& GLShaderProgram::SetUniformFloat (const std::string& uniformName, float uniformValue)
{
    GLuint location = GetUniformLocationId (uniformName);
    glUniform1f (location, uniformValue);

    return *this;
}

ShaderProgram& GLShaderProgram::SetUniformVector2f (const std::string& uniformName, const Vector2f& uniformValue)
{
    GLuint location = GetUniformLocationId (uniformName);
    glUniform2fv (location, 1, &uniformValue[0]);

    return *this;
}

ShaderProgram& GLShaderProgram::SetUniformVector3f (const std::string& uniformName, const Vector3f& uniformValue)
{
    GLuint location = GetUniformLocationId (uniformName);
    glUniform3fv (location, 1, &uniformValue[0]);

    return *this;
}

ShaderProgram& GLShaderProgram::SetUniformVector4f (const std::string& uniformName, const Vector4f& uniformValue)
{
    GLuint location = GetUniformLocationId (uniformName);
    glUniform4fv (location, 1, &uniformValue[0]);

    return *this;
}

void GLShaderProgram::Link ()
{
    GLint success;
    char errorLog[512];

    glLinkProgram (shaderProgramId);
    glGetProgramiv (shaderProgramId, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog (shaderProgramId, 512, nullptr, errorLog);
        glDeleteProgram (shaderProgramId);
        LogMessage () << errorLog;
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unable to link program" << shaderProgramId;
    }
}

void GLShaderProgram::Use () const
{
    glUseProgram (shaderProgramId);
}

void GLShaderProgram::AttachShader (const Shader& shader)
{
    const GLShader* glShader = static_cast<const GLShader*> (&shader);

    glAttachShader (shaderProgramId, glShader->GetShaderId ());
}


