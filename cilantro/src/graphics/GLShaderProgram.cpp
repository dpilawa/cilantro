#include "graphics/GLShaderProgram.h"
#include "graphics/GLShader.h"
#include "system/LogMessage.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include "math/Vector4f.h"
#include "math/Matrix3f.h"
#include "math/Matrix4f.h"

namespace cilantro {

GLShaderProgram::GLShaderProgram () 
    : ShaderProgram ()
{
    m_glShaderProgramId = glCreateProgram ();
}

void GLShaderProgram::AttachShader (const std::shared_ptr<IShader> shader)
{
    auto glShader = std::static_pointer_cast<GLShader> (shader);

    glAttachShader (m_glShaderProgramId, glShader->GetShaderId ());
}

void GLShaderProgram::Link ()
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

bool GLShaderProgram::HasUniform (const std::string& uniformName) const
{
    GLuint location = GetUniformLocationId (uniformName);

    if (location == GL_INVALID_INDEX)
    {
        return false;
    }

    return true;
}

IShaderProgram& GLShaderProgram::SetUniformFloat (const std::string& uniformName, float uniformValue)
{
    GLuint location = GetUniformLocationId (uniformName);
    if (location != GL_INVALID_INDEX)
    {
        glUniform1f (location, uniformValue);
    }

    return *this;
}

IShaderProgram& GLShaderProgram::SetUniformFloatv (const std::string& uniformName, const float* uniformValue, size_t count)
{
    GLuint location = GetUniformLocationId (uniformName);
    if (location != GL_INVALID_INDEX)
    {
        glUniform1fv (location, static_cast<GLsizei> (count), uniformValue);
    }

    return *this;
}

IShaderProgram& GLShaderProgram::SetUniformVector2f (const std::string& uniformName, const Vector2f& uniformValue)
{
    GLuint location = GetUniformLocationId (uniformName);
    if (location != GL_INVALID_INDEX)
    {
        glUniform2fv (location, 1, &uniformValue[0]);
    }
    return *this;
}

IShaderProgram& GLShaderProgram::SetUniformVector3f (const std::string& uniformName, const Vector3f& uniformValue)
{
    GLuint location = GetUniformLocationId (uniformName);
    if (location != GL_INVALID_INDEX)
    {
        glUniform3fv (location, 1, &uniformValue[0]);
    }

    return *this;
}

IShaderProgram& GLShaderProgram::SetUniformVector4f (const std::string& uniformName, const Vector4f& uniformValue)
{
    GLuint location = GetUniformLocationId (uniformName);
    if (location != GL_INVALID_INDEX)
    {
        glUniform4fv (location, 1, &uniformValue[0]);
    }

    return *this;
}

IShaderProgram& GLShaderProgram::SetUniformMatrix3f (const std::string& uniformName, const Matrix3f& uniformValue)
{
    GLuint location = GetUniformLocationId (uniformName);
    if (location != GL_INVALID_INDEX)
    {
        glUniformMatrix3fv (location, 1, GL_TRUE, uniformValue[0]);
    }

    return *this;
}

IShaderProgram& GLShaderProgram::SetUniformMatrix4f (const std::string& uniformName, const Matrix4f& uniformValue)
{
    GLuint location = GetUniformLocationId (uniformName);
    if (location != GL_INVALID_INDEX)
    {
        glUniformMatrix4fv (location, 1, GL_TRUE, uniformValue[0]);
    }

    return *this;
}

IShaderProgram& GLShaderProgram::SetUniformMatrix3fv (const std::string& uniformName, const float* uniformValue, size_t count)
{
    GLuint location = GetUniformLocationId (uniformName);
    if (location != GL_INVALID_INDEX)
    {
        glUniformMatrix3fv (location, static_cast<GLsizei> (count), GL_TRUE, uniformValue);
    }

    return *this;
}

IShaderProgram& GLShaderProgram::SetUniformMatrix4fv (const std::string& uniformName, const float* uniformValue, size_t count)
{
    GLuint location = GetUniformLocationId (uniformName);
    if (location != GL_INVALID_INDEX)
    {
        glUniformMatrix4fv (location, static_cast<GLsizei> (count), GL_TRUE, uniformValue);
    }

    return *this;
}

void GLShaderProgram::Use () const
{
    glUseProgram (m_glShaderProgramId);
}

GLuint GLShaderProgram::GetProgramId () const
{
    return m_glShaderProgramId;
}

GLuint GLShaderProgram::GetUniformLocationId (const std::string& uniformName) const
{
    this->Use ();
    GLuint paramUniformLocation = glGetUniformLocation (m_glShaderProgramId, uniformName.c_str ());

    if (paramUniformLocation == GL_INVALID_INDEX)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Uniform" << uniformName << "not found in stage shader" << this->GetName ();
    }

    return paramUniformLocation;
}

void GLShaderProgram::BindUniformBlock (const std::string& blockName, EBindingPoint bp)
{
    GLuint uniformBlockIndex = glGetUniformBlockIndex (m_glShaderProgramId, blockName.c_str ());

    if (uniformBlockIndex != GL_INVALID_INDEX)
    {
        glUniformBlockBinding (m_glShaderProgramId, uniformBlockIndex, bp);
    }	
    else
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Uniform block" << blockName << "not found in stage shader" << this->GetName ();
    }
    
}

} // namespace cilantro






