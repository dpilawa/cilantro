#include "graphics/GLShader.h"
#include "graphics/GLRenderer.h"
#include "graphics/GLUtils.h"
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
        case EShaderType::COMPUTE_SHADER:
            m_glShaderId = glCreateShader (GL_COMPUTE_SHADER);
            break;
    }

    SetDefaultParameters ();
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

void GLShader::SetDefaultParameters ()
{
    Shader::SetDefaultParameters ();

    SetParameter ("%%CILANTRO_GLSL_VERSION%%", GLUtils::GetGLSLVersion ().directive);

    SetParameter ("%%UBO_MATRICES%%", std::to_string (static_cast<int> (EGlUBOType::UBO_MATRICES)));
    SetParameter ("%%UBO_POINTLIGHTS%%", std::to_string (static_cast<int> (EGlUBOType::UBO_POINTLIGHTS)));
    SetParameter ("%%UBO_DIRECTIONALLIGHTS%%", std::to_string (static_cast<int> (EGlUBOType::UBO_DIRECTIONALLIGHTS)));
    SetParameter ("%%UBO_SPOTLIGHTS%%", std::to_string (static_cast<int> (EGlUBOType::UBO_SPOTLIGHTS)));
    SetParameter ("%%UBO_DIRECTIONALLIGHTVIEWMATRICES%%", std::to_string (static_cast<int> (EGlUBOType::UBO_DIRECTIONALLIGHTVIEWMATRICES)));
    SetParameter ("%%UBO_BONETRANSFORMATIONS%%", std::to_string (static_cast<int> (EGlUBOType::UBO_BONETRANSFORMATIONS)));

    SetParameter ("%%SSBO_VERTICES%%", std::to_string (static_cast<int> (EGlSSBOType::SSBO_VERTICES)));
    SetParameter ("%%SSBO_BONEINDICES%%", std::to_string (static_cast<int> (EGlSSBOType::SSBO_BONEINDICES)));
    SetParameter ("%%SSBO_BONEWEIGHTS%%", std::to_string (static_cast<int> (EGlSSBOType::SSBO_BONEWEIGHTS)));
    SetParameter ("%%SSBO_AABB%%", std::to_string (static_cast<int> (EGlSSBOType::SSBO_AABB)));
}

GLuint GLShader::GetShaderId () const
{
    return m_glShaderId;
}

} // namespace cilantro

