#include "graphics/Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

CShader::CShader (const std::string& path, EShaderType shaderType) : CLoadableResource (path)
{
    this->m_shaderType = shaderType;
    Load (path);
}

CShader::~CShader ()
{
}

void CShader::SetParameter (const std::string& parameter, const std::string& value)
{
    std::size_t pos;

    pos = m_shaderSource.find (parameter);
    if (pos != std::string::npos)
    {
        m_shaderSource.replace (pos, parameter.length (), value);
    }
}

void CShader::Load (const std::string& path)
{
    std::ifstream f (path, std::ios::binary);
    std::ostringstream ss;
   
    if (!f.is_open ())
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unable to read shader file" << path;
    }

    ss << f.rdbuf ();
    m_shaderSource = ss.str ();

    this->SetDefaultParameters ();
}

void CShader::SetDefaultParameters ()
{
    SetParameter ("%%CILANTRO_GL_VERSION%%", std::to_string (CILANTRO_GL_VERSION));
    SetParameter ("%%CILANTRO_MAX_BONES%%", std::to_string (CILANTRO_MAX_BONES));
    SetParameter ("%%CILANTRO_MAX_BONE_INFLUENCES%%", std::to_string (CILANTRO_MAX_BONE_INFLUENCES));
    SetParameter ("%%CILANTRO_MAX_POINT_LIGHTS%%", std::to_string (CILANTRO_MAX_POINT_LIGHTS));
    SetParameter ("%%CILANTRO_MAX_SPOT_LIGHTS%%", std::to_string (CILANTRO_MAX_SPOT_LIGHTS));
    SetParameter ("%%CILANTRO_MAX_DIRECTIONAL_LIGHTS%%", std::to_string (CILANTRO_MAX_DIRECTIONAL_LIGHTS));
}