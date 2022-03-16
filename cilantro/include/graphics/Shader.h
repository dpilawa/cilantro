#pragma once

#include "cilantroengine.h"
#include "graphics/IShader.h"
#include "resource/LoadableResource.h"
#include <string>

class CShader : public IShader, public CLoadableResource
{
public:
    CShader (const std::string& path, EShaderType shaderType);
    virtual ~CShader () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual void Load (const std::string& path) override;
    
    void SetParameter (const std::string& parameter, const std::string& value) override;

    ///////////////////////////////////////////////////////////////////////////

private:
    void SetDefaultParameters ();

protected:

    // shader type (vertex, fragment)
    EShaderType m_shaderType;

    // source code
    std::string m_shaderSource;
};


