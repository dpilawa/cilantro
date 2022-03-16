#include "graphics/GLShaderProgram.h"
#include "graphics/GLShader.h"
#include "system/LogMessage.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include "math/Vector4f.h"

CGLShaderProgram::CGLShaderProgram () 
    : CShaderProgram ()
{
    m_glShaderProgramId = glCreateProgram ();
}

void CGLShaderProgram::AttachShader (const IShader& shader)
{
    const CGLShader* glShader = static_cast<const CGLShader*> (&shader);

    glAttachShader (m_glShaderProgramId, glShader->GetShaderId ());
}

void CGLShaderProgram::Link ()
{
    GLint success;
    char errorLog[512];

    glLinkProgram (m_glShaderProgramId);
    glGetProgramiv (m_glShaderProgramId, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog (m_glShaderProgramId, 512, nullptr, errorLog);
        glDeleteProgram (m_glShaderProgramId);
        LogMessage () << errorLog;
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unable to link program" << m_glShaderProgramId;
    }
}

IShaderProgram& CGLShaderProgram::SetUniformFloat (const std::string& uniformName, float uniformValue)
{
    GLuint location = GetUniformLocationId (uniformName);
    glUniform1f (location, uniformValue);

    return *this;
}

IShaderProgram& CGLShaderProgram::SetUniformVector2f (const std::string& uniformName, const Vector2f& uniformValue)
{
    GLuint location = GetUniformLocationId (uniformName);
    glUniform2fv (location, 1, &uniformValue[0]);

    return *this;
}

IShaderProgram& CGLShaderProgram::SetUniformVector3f (const std::string& uniformName, const Vector3f& uniformValue)
{
    GLuint location = GetUniformLocationId (uniformName);
    glUniform3fv (location, 1, &uniformValue[0]);

    return *this;
}

IShaderProgram& CGLShaderProgram::SetUniformVector4f (const std::string& uniformName, const Vector4f& uniformValue)
{
    GLuint location = GetUniformLocationId (uniformName);
    glUniform4fv (location, 1, &uniformValue[0]);

    return *this;
}

void CGLShaderProgram::Use () const
{
    glUseProgram (m_glShaderProgramId);
}

GLuint CGLShaderProgram::GetProgramId () const
{
    return m_glShaderProgramId;
}

GLuint CGLShaderProgram::GetUniformLocationId (const std::string& uniformName) const
{
    this->Use ();
    GLuint paramUniformLocation = glGetUniformLocation (m_glShaderProgramId, uniformName.c_str ());

    if (paramUniformLocation == GL_INVALID_INDEX)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Uniform not found in stage shader:" << uniformName;
    }

    return paramUniformLocation;
}

void CGLShaderProgram::BindUniformBlock (const std::string& blockName, EBindingPoint bp)
{
    GLuint uniformBlockIndex = glGetUniformBlockIndex (m_glShaderProgramId, blockName.c_str ());

    if (uniformBlockIndex != GL_INVALID_INDEX)
    {
        glUniformBlockBinding (m_glShaderProgramId, uniformBlockIndex, bp);
    }	
    else
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Shader" << this->GetName () << "has no uniform" << blockName;
    }
    
}






