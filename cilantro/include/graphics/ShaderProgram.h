#pragma once

#include "cilantroengine.h"
#include "graphics/IShaderProgram.h"
#include "resource/Resource.h"

namespace cilantro {

class CShaderProgram : public IShaderProgram, public Resource
{
public:
    CShaderProgram () : Resource () {}; 
    virtual ~CShaderProgram () {};
};

} // namespace cilantro
