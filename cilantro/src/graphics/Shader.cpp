#include "cilantroengine.h"
#include "graphics/Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader (const std::string& path, ShaderType shaderType) : LoadableResource (path)
{
    this->shaderType = shaderType;
    Load (path);
}

Shader::~Shader ()
{
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
}