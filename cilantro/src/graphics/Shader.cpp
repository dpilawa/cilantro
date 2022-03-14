#include "graphics/Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader (const std::string& path, ShaderType shaderType) : CLoadableResource (path)
{
    this->shaderType = shaderType;
    Load (path);
}

Shader::~Shader ()
{
}

void Shader::SetParameter (const std::string& parameter, const std::string& value)
{
    std::size_t pos;

    pos = shaderSourceCode.find (parameter);
    if (pos != std::string::npos)
    {
        shaderSourceCode.replace (pos, parameter.length (), value);
    }
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
    shaderSourceCode = ss.str ();

    this->SetDefaultParameters ();
}

void Shader::SetDefaultParameters ()
{
    SetParameter ("%%CILANTRO_GL_VERSION%%", std::to_string (CILANTRO_GL_VERSION));
    SetParameter ("%%CILANTRO_MAX_BONES%%", std::to_string (CILANTRO_MAX_BONES));
    SetParameter ("%%CILANTRO_MAX_BONE_INFLUENCES%%", std::to_string (CILANTRO_MAX_BONE_INFLUENCES));
    SetParameter ("%%CILANTRO_MAX_POINT_LIGHTS%%", std::to_string (CILANTRO_MAX_POINT_LIGHTS));
    SetParameter ("%%CILANTRO_MAX_SPOT_LIGHTS%%", std::to_string (CILANTRO_MAX_SPOT_LIGHTS));
    SetParameter ("%%CILANTRO_MAX_DIRECTIONAL_LIGHTS%%", std::to_string (CILANTRO_MAX_DIRECTIONAL_LIGHTS));
}