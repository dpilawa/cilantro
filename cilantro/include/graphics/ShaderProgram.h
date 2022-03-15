#pragma once

#include "cilantroengine.h"
#include "resource/Resource.h"
#include <string>

class CShader;
class Vector2f;
class Vector3f;
class Vector4f;

class CShaderProgram : public CResource
{
public:
    __EAPI CShaderProgram ();
    __EAPI virtual ~CShaderProgram ();

    // uniform manipulation
    __EAPI virtual CShaderProgram& SetUniformFloat (const std::string& uniformName, float uniformValue) = 0;
    __EAPI virtual CShaderProgram& SetUniformVector2f (const std::string& uniformName, const Vector2f& uniformValue) = 0;
    __EAPI virtual CShaderProgram& SetUniformVector3f (const std::string& uniformName, const Vector3f& uniformValue) = 0;
    __EAPI virtual CShaderProgram& SetUniformVector4f (const std::string& uniformName, const Vector4f& uniformValue) = 0;

     // attach shader to a program
    __EAPI CShaderProgram& AddShader (const std::string& shaderName);   
    __EAPI virtual void Link () = 0;

    // use program
    virtual void Use () const = 0;

private:
    // attach shader to a program
    virtual void AttachShader (const CShader& shader) = 0;
};
