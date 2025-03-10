#include "graphics/GLShader.h"
#include "system/LogMessage.h"

#include <string>

namespace cilantro {

GLShader::GLShader (const std::string& path, EShaderType shaderType) : Shader (path, shaderType)
{
    GLint success;
    char errorLog[512];

    switch (shaderType)
    {
        case EShaderType::VERTEX_SHADER:
            m_glShaderId = glCreateShader (GL_VERTEX_SHADER);
            break;
        case EShaderType::GEOMETRY_SHADER:
            m_glShaderId = glCreateShader (GL_GEOMETRY_SHADER);
            break;
        case EShaderType::FRAGMENT_SHADER:
            m_glShaderId = glCreateShader (GL_FRAGMENT_SHADER);
            break;
    }

    Compile ();

    glGetShaderiv (m_glShaderId, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog (m_glShaderId, 512, nullptr, errorLog);
        glDeleteShader (m_glShaderId);
        LogMessage () << m_shaderSource;
        LogMessage () << errorLog;
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unable to compile shader" << m_glShaderId << path;
    }
}

void GLShader::Compile ()
{
    const char* src;

    src = m_shaderSource.c_str ();
    glShaderSource (m_glShaderId, 1, &src, NULL);
    glCompileShader (m_glShaderId); 
}

GLuint GLShader::GetShaderId () const
{
    return m_glShaderId;
}

} // namespace cilantro

