#pragma once

#include "cilantroengine.h"
#include "graphics/IShader.h"
#include "resource/LoadableResource.h"
#include <string>
#include <map>

namespace cilantro {

typedef std::map<std::string, std::string> TParameterValueMap;

class __CEAPI Shader : public IShader, public LoadableResource
{
public:
    Shader (const std::string& path, EShaderType shaderType);
    virtual ~Shader () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual void Load (const std::string& path) override;
    
    virtual void SetParameter (const std::string& parameter, const std::string& value) override;
    virtual void SetDefaultParameters () override;

    ///////////////////////////////////////////////////////////////////////////

protected:

    // shader type (vertex, fragment)
    EShaderType m_shaderType;

    // source code
    std::string m_shaderSource;
    std::string m_shaderSourceParametrized;

    // parameter value map
    TParameterValueMap m_paramValMap;

};

} // namespace cilantro


