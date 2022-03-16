#pragma once

#include "cilantroengine.h"
#include "graphics/IShaderProgram.h"
#include "resource/Resource.h"

class CShaderProgram : public IShaderProgram, public CResource
{
public:
    CShaderProgram () : CResource () {}; 
    virtual ~CShaderProgram () {};
};
