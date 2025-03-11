#pragma once

#include "cilantroengine.h"
#include "graphics/IShaderProgram.h"
#include "resource/Resource.h"

namespace cilantro {

class __CEAPI ShaderProgram : public IShaderProgram, public Resource
{
public:
    ShaderProgram () : Resource () {}; 
    virtual ~ShaderProgram () {};
};

} // namespace cilantro
