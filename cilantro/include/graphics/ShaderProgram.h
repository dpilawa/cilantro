#ifndef _SHADERPROGRAM_H_
#define _SHADERPROGRAM_H_

#include "cilantroengine.h"
#include "resource/Resource.h"
#include "graphics/Shader.h"
#include <string>

class ShaderProgram : public Resource
{
public:
    ShaderProgram ();
    virtual ~ShaderProgram ();

     // attach shader to a program
    __EAPI ShaderProgram& AddShader (const std::string& shaderName);   
    __EAPI virtual void Link () = 0;

    // use program
    virtual void Use () = 0;

private:
    // attach shader to a program
    virtual void AttachShader (const Shader& shader) = 0;
};

#endif