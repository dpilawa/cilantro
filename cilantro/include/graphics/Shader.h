#ifndef _SHADER_H_
#define _SHADER_H_

#include "cilantroengine.h"
#include "resource/LoadableResource.h"
#include <string>

enum class ShaderType { VERTEX_SHADER, FRAGMENT_SHADER };

class Shader : public LoadableResource
{
public:
    Shader (const std::string& path, ShaderType shaderType);
    virtual ~Shader ();

protected:
    // shader type (vertex, fragment)
    ShaderType shaderType;

    // source code
    std::string shaderSourceCode;

private:

    void Load (const std::string& path);

};

#endif
