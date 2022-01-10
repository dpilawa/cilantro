#ifndef _SHADER_H_
#define _SHADER_H_

#include "cilantroengine.h"
#include "resource/LoadableResource.h"
#include <string>
#include <unordered_map>

enum class ShaderType { VERTEX_SHADER, GEOMETRY_SHADER, FRAGMENT_SHADER };

class Shader : public LoadableResource
{
public:
    Shader (const std::string& path, ShaderType shaderType);
    virtual ~Shader ();

    void SetParameter (std::string parameter, std::string value);

protected:
    // shader type (vertex, fragment)
    ShaderType shaderType;

    // source code
    std::string shaderSourceCode;

private:
    void Load (const std::string& path);
    void SetDefaultParameters ();

    virtual void Compile() = 0;

};

#endif
