#ifndef _SHADERPROGRAM_H_
#define _SHADERPROGRAM_H_

#include "cilantroengine.h"
#include "resource/Resource.h"
#include "graphics/Shader.h"
#include <string>

class Vector2f;
class Vector3f;
class Vector4f;

class ShaderProgram : public Resource
{
public:
    __EAPI ShaderProgram ();
    __EAPI virtual ~ShaderProgram ();

    // uniform manipulation
    __EAPI virtual ShaderProgram& SetUniformFloat (const std::string& uniformName, float uniformValue) = 0;
    __EAPI virtual ShaderProgram& SetUniformVector2f (const std::string& uniformName, const Vector2f& uniformValue) = 0;
    __EAPI virtual ShaderProgram& SetUniformVector3f (const std::string& uniformName, const Vector3f& uniformValue) = 0;
    __EAPI virtual ShaderProgram& SetUniformVector4f (const std::string& uniformName, const Vector4f& uniformValue) = 0;

     // attach shader to a program
    __EAPI ShaderProgram& AddShader (const std::string& shaderName);   
    __EAPI virtual void Link () = 0;

    // use program
    virtual void Use () const = 0;

private:
    // attach shader to a program
    virtual void AttachShader (const Shader& shader) = 0;
};

#endif