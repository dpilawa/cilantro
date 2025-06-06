#pragma once

#include "cilantroengine.h"
#include "graphics/IShader.h"
#include "resource/LoadableResource.h"
#include <string>
#include <unordered_map>

namespace cilantro {

typedef std::unordered_map<std::string, std::string> TValueMap;

class __CEAPI Shader : public IShader, public LoadableResource
{
public:
    Shader (const std::string& path, EShaderType shaderType);
    virtual ~Shader () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual void Load (const std::string& path) override;
    
    virtual void SetStaticParameter (const std::string& parameter, const std::string& value) override;
    virtual void SetVariable (const std::string& variable, const std::string& value) override;
    virtual void SetDefaults () override;

    ///////////////////////////////////////////////////////////////////////////

protected:

    // shader type (vertex, fragment)
    EShaderType m_shaderType;

    // source code
    std::string m_shaderSourceNoVariables;
    std::string m_shaderSource;

    // parameter and variable value maps
    TValueMap m_parameterValMap;
    TValueMap m_variableValMap;

private:

    // function to replace variables in the shader source code
    void ReplaceVariables ();

};

} // namespace cilantro


