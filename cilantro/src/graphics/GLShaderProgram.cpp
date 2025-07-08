#include "graphics/GLShaderProgram.h"
#include "graphics/GLShader.h"
#include "graphics/GLUtils.h"
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

    glLinkProgram (m_glShaderProgramId);
    glGetProgramiv (m_glShaderProgramId, GL_LINK_STATUS, &success);

    if (!success)
    {
        GLint length;
        glGetProgramiv (m_glShaderProgramId, GL_INFO_LOG_LENGTH, &length);
        std::string errorLog(length, ' ');

        glGetProgramInfoLog (m_glShaderProgramId, length, nullptr, &errorLog[0]);
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

IShaderProgram& GLShaderProgram::SetUniformInt (const std::string& uniformName, int uniformValue)
{
    GLuint location = GetUniformLocationId (uniformName);
    if (location != GL_INVALID_INDEX)
    {
        glUniform1i (location, uniformValue);
    }

    return *this;
}

IShaderProgram& GLShaderProgram::SetUniformUInt (const std::string& uniformName, unsigned int uniformValue)
{
    GLuint location = GetUniformLocationId (uniformName);
    if (location != GL_INVALID_INDEX)
    {
        glUniform1ui (location, uniformValue);
    }

    return *this;
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

void GLShaderProgram::Compute (unsigned int groupsX, unsigned int groupsY, unsigned int groupsZ) const
{
    glDispatchCompute (groupsX, groupsY, groupsZ);
    glMemoryBarrier (GL_SHADER_STORAGE_BARRIER_BIT);
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
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Uniform" << uniformName << "not found in shader program" << this->GetName ();
    }

    return paramUniformLocation;
}

void GLShaderProgram::BindUniformBlock (const std::string& blockName, EGlUBOType bp)
{
    GLuint uniformBlockIndex = glGetUniformBlockIndex (m_glShaderProgramId, blockName.c_str ());
    GLint actualBinding = -1;

    if (uniformBlockIndex != GL_INVALID_INDEX)
    {
        glUniformBlockBinding (m_glShaderProgramId, uniformBlockIndex, static_cast<int>(bp));
        glGetActiveUniformBlockiv(m_glShaderProgramId, uniformBlockIndex, GL_UNIFORM_BLOCK_BINDING, &actualBinding);
        if (actualBinding != static_cast<int>(bp))
        {
            LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unable to bind uniform block" << blockName << "to binding point" << static_cast<int>(bp);
        }
    }	
    else
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Uniform block" << blockName << "not found in shader program" << this->GetName ();
    }
    
}

void GLShaderProgram::BindShaderStorageBlock(const std::string& blockName, EGlSSBOType bp)
{
    if (GLUtils::GetGLSLVersion ().versionNumber >= 430)
    {
        GLuint blockIndex = glGetProgramResourceIndex(m_glShaderProgramId, GL_SHADER_STORAGE_BLOCK, blockName.c_str());
        GLint actualBinding = -1;

        if (blockIndex != GL_INVALID_INDEX)
        {
            glShaderStorageBlockBinding(m_glShaderProgramId, blockIndex, static_cast<GLuint>(bp));

            GLenum props[] = { GL_BUFFER_BINDING };
            GLint values[1];
            glGetProgramResourceiv(m_glShaderProgramId, GL_SHADER_STORAGE_BLOCK, blockIndex, 1, props, 1, nullptr, values);
            actualBinding = values[0];

            if (actualBinding != static_cast<GLint>(bp))
            {
                LogMessage(MSG_LOCATION, EXIT_FAILURE)
                    << "Unable to bind SSBO block \"" << blockName << "\" to binding point " << static_cast<int>(bp);
            }
        }
        else
        {
            LogMessage(MSG_LOCATION, EXIT_FAILURE)
                << "SSBO block \"" << blockName << "\" not found in shader program \"" << this->GetName() << "\"";
        }
    }
    else
    {
        LogMessage(MSG_LOCATION, EXIT_FAILURE) << "SSBO binding not supported in OpenGL version < 4.3";
    }
}


} // namespace cilantro






