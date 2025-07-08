#include "graphics/Shader.h"
#include "graphics/ShaderProcessor.h"
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
}

void Shader::Load (const std::string& path)
{
    ShaderProcessor processor (m_parameterValMap);
    m_shaderSourceNoVariables = processor.ProcessShader (path);
    ReplaceVariables ();
}

void Shader::SetStaticParameter (const std::string& parameter, const std::string& value)
{
    m_parameterValMap.insert_or_assign (parameter, value);
}

void Shader::SetVariable (const std::string& variable, const std::string& value)
{
    m_variableValMap.insert_or_assign (variable, value);
    ReplaceVariables ();
}

void Shader::SetDefaults ()
{  
    SetStaticParameter ("CILANTRO_MAX_BONES", std::to_string (CILANTRO_MAX_BONES));
    SetStaticParameter ("CILANTRO_MAX_BONE_INFLUENCES", std::to_string (CILANTRO_MAX_BONE_INFLUENCES));
    SetStaticParameter ("CILANTRO_MAX_POINT_LIGHTS", std::to_string (CILANTRO_MAX_POINT_LIGHTS));
    SetStaticParameter ("CILANTRO_MAX_SPOT_LIGHTS", std::to_string (CILANTRO_MAX_SPOT_LIGHTS));
    SetStaticParameter ("CILANTRO_MAX_DIRECTIONAL_LIGHTS", std::to_string (CILANTRO_MAX_DIRECTIONAL_LIGHTS));
    SetStaticParameter ("CILANTRO_MAX_SPOT_LIGHTS", std::to_string (CILANTRO_MAX_SPOT_LIGHTS));
    SetStaticParameter ("CILANTRO_COMPUTE_GROUP_SIZE", std::to_string (CILANTRO_COMPUTE_GROUP_SIZE));

    SetStaticParameter ("CILANTRO_SHADOW_MAP_BINDING", std::to_string (CILANTRO_SHADOW_MAP_BINDING));
    SetStaticParameter ("CILANTRO_SHADOW_BIAS", std::to_string (CILANTRO_SHADOW_BIAS));

    SetVariable ("ACTIVE_DIRECTIONAL_LIGHTS", "1");
    SetVariable ("ACTIVE_SPOT_LIGHTS", "1");
    SetVariable ("ACTIVE_POINT_LIGHTS", "1");
}

void Shader::ReplaceVariables ()
{
    m_shaderSource = m_shaderSourceNoVariables;

    for (const auto& [variable, value] : m_variableValMap)
    {
        std::string var = "$$" + variable + "$$";
        size_t pos = 0;
        while ((pos = m_shaderSource.find (var, pos)) != std::string::npos)
        {
            m_shaderSource.replace (pos, var.length (), value);
            pos += value.length ();
        }
    }
}

} // namespace cilantro