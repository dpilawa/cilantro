#include "graphics/Shader.h"
#include "system/LogMessage.h"
#include "system/Game.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

CShader::CShader (const std::string& path, EShaderType shaderType) : CLoadableResource (path)
{
    this->m_shaderType = shaderType;
    Load (path);
}

void CShader::Load (const std::string& path)
{
    std::string absolutePath = CGame::GetPath() + "/" + path;
    std::ifstream f (absolutePath, std::ios::binary);
    std::ostringstream ss;
   
    if (!f.is_open ())
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unable to read shader file" << absolutePath;
    }

    ss << f.rdbuf ();
    m_shaderSourceParametrized = ss.str ();
    m_shaderSource = m_shaderSourceParametrized;

    this->SetDefaultParameters ();
}

void CShader::SetParameter (const std::string& parameter, const std::string& value)
{
    std::size_t pos;

    m_paramValMap.insert_or_assign (parameter, value);

    m_shaderSource = m_shaderSourceParametrized;

    for (auto& p : m_paramValMap)
    {
        while ((pos = m_shaderSource.find (p.first)) != std::string::npos)
        {
            m_shaderSource.replace (pos, p.first.length (), p.second);
        }
    }    
}

void CShader::SetDefaultParameters ()
{
#ifdef CILANTRO_BUILDING_GLES    
    SetParameter ("%%CILANTRO_GL_VERSION%%", CILANTRO_GLES_VERSION);
#else
    SetParameter ("%%CILANTRO_GL_VERSION%%", std::to_string (CILANTRO_GL_VERSION));
#endif    
    SetParameter ("%%CILANTRO_MAX_BONES%%", std::to_string (CILANTRO_MAX_BONES));
    SetParameter ("%%CILANTRO_MAX_BONE_INFLUENCES%%", std::to_string (CILANTRO_MAX_BONE_INFLUENCES));
    SetParameter ("%%CILANTRO_MAX_POINT_LIGHTS%%", std::to_string (CILANTRO_MAX_POINT_LIGHTS));
    SetParameter ("%%CILANTRO_MAX_SPOT_LIGHTS%%", std::to_string (CILANTRO_MAX_SPOT_LIGHTS));
    SetParameter ("%%CILANTRO_MAX_DIRECTIONAL_LIGHTS%%", std::to_string (CILANTRO_MAX_DIRECTIONAL_LIGHTS));

    SetParameter ("%%ACTIVE_DIRECTIONAL_LIGHTS%%", "1");
}