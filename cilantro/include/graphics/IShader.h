#pragma once

#include "cilantroengine.h"
#include <string>

namespace cilantro {

enum class EShaderType { VERTEX_SHADER, GEOMETRY_SHADER, FRAGMENT_SHADER, COMPUTE_SHADER };

struct IShader 
{
    virtual ~IShader () {};

    virtual void Compile () = 0;

    virtual void SetStaticParameter (const std::string& parameter, const std::string& value) = 0;
    virtual void SetVariable (const std::string& variable, const std::string& value) = 0;
    virtual void SetDefaults () = 0;
};

} // namespace cilantro