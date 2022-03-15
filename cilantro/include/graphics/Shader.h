#pragma once

#include "cilantroengine.h"
#include "resource/LoadableResource.h"

enum class EShaderType { VERTEX_SHADER, GEOMETRY_SHADER, FRAGMENT_SHADER };

class CShader : public CLoadableResource
{
public:
    CShader (const std::string& path, EShaderType shaderType);
    virtual ~CShader ();

    void SetParameter (const std::string& parameter, const std::string& value);

private:
    void Load (const std::string& path);
    void SetDefaultParameters ();

    virtual void Compile() = 0;

protected:
    // shader type (vertex, fragment)
    EShaderType m_shaderType;

    // source code
    std::string m_shaderSource;
};


