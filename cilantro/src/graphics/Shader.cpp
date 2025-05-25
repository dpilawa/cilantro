#include "graphics/Shader.h"
#include "system/LogMessage.h"
#include "system/Game.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace cilantro {

Shader::Shader (const std::string& path, EShaderType shaderType) : LoadableResource (path)
{
    this->m_shaderType = shaderType;
    Load (path);
}

void Shader::Load (const std::string& path)
{
    std::ifstream f (path, std::ios::binary);
    std::ostringstream ss;
   
    if (!f.is_open ())
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unable to read shader file" << path;
    }

    ss << f.rdbuf ();
    m_shaderSourceParametrized = ss.str ();
    m_shaderSource = m_shaderSourceParametrized;
}

void Shader::SetParameter (const std::string& parameter, const std::string& value)
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

void Shader::SetDefaultParameters ()
{  
    SetParameter ("%%CILANTRO_MAX_BONES%%", std::to_string (CILANTRO_MAX_BONES));
    SetParameter ("%%CILANTRO_MAX_BONE_INFLUENCES%%", std::to_string (CILANTRO_MAX_BONE_INFLUENCES));
    SetParameter ("%%CILANTRO_MAX_POINT_LIGHTS%%", std::to_string (CILANTRO_MAX_POINT_LIGHTS));
    SetParameter ("%%CILANTRO_MAX_SPOT_LIGHTS%%", std::to_string (CILANTRO_MAX_SPOT_LIGHTS));
    SetParameter ("%%CILANTRO_MAX_DIRECTIONAL_LIGHTS%%", std::to_string (CILANTRO_MAX_DIRECTIONAL_LIGHTS));
    SetParameter ("%%CILANTRO_COMPUTE_GROUP_SIZE%%", std::to_string (CILANTRO_COMPUTE_GROUP_SIZE));

    SetParameter ("%%ACTIVE_DIRECTIONAL_LIGHTS%%", "1");
}

} // namespace cilantro