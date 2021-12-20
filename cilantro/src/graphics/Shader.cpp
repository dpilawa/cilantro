#include "cilantroengine.h"
#include "graphics/Shader.h"
#include "resource/Mesh.h"
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
    std::size_t pos;
    std::ifstream f (path, std::ios::binary);
    std::ostringstream ss;
    std::string versionTag ("%%CILANTRO_GL_VERSION%%");
    std::string boneMaxTag ("%%CILANTRO_MAX_BONES%%");
    std::string boneMaxInfluenceTag ("%%CILANTRO_MAX_BONE_INFLUENCES%%");
   
    if (!f.is_open ())
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unable to read shader file" << path;
    }

    ss << f.rdbuf ();
    shaderSourceCode = ss.str ();

    // replace tags in shader source
    pos = shaderSourceCode.find (versionTag);
    if (pos != std::string::npos)
    {
        shaderSourceCode.replace (pos, versionTag.length (), std::to_string (CILANTRO_GL_VERSION));
    }
    else
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Version tag not found in shader" << path;    
    }

    pos = shaderSourceCode.find (boneMaxTag);
    if (pos != std::string::npos)
    {
        shaderSourceCode.replace (pos, boneMaxTag.length (), std::to_string (CILANTRO_MAX_BONES));
    }  

    pos = shaderSourceCode.find (boneMaxInfluenceTag);
    if (pos != std::string::npos)
    {
        shaderSourceCode.replace (pos, boneMaxInfluenceTag.length (), std::to_string (CILANTRO_MAX_BONE_INFLUENCES));
    }

    LogMessage (MSG_LOCATION) << "Loaded shader" << path << shaderSourceCode.length () << "bytes";
}