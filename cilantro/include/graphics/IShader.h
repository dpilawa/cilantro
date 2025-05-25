#pragma once

#include "cilantroengine.h"
#include <string>

namespace cilantro {

enum class EShaderType { VERTEX_SHADER, GEOMETRY_SHADER, FRAGMENT_SHADER, COMPUTE_SHADER };

struct IShader 
{
    virtual ~IShader () {};

    virtual void Compile () = 0;

    virtual void SetParameter (const std::string& parameter, const std::string& value) = 0;
    virtual void SetDefaultParameters () = 0;
};

} // namespace cilantro