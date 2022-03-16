#pragma once

#include "cilantroengine.h"
#include <string>

enum EBindingPoint { BP_MATRICES = 1, BP_POINTLIGHTS, BP_DIRECTIONALLIGHTS, BP_SPOTLIGHTS };

struct IShader;
class Vector2f;
class Vector3f;
class Vector4f;

struct IShaderProgram 
{
    virtual ~IShaderProgram () {};

    // linking
    virtual void AttachShader (const IShader& shader) = 0;  
    virtual void Link () = 0;

    // uniform manipulation
    virtual IShaderProgram& SetUniformFloat (const std::string& uniformName, float uniformValue) = 0;
    virtual IShaderProgram& SetUniformVector2f (const std::string& uniformName, const Vector2f& uniformValue) = 0;
    virtual IShaderProgram& SetUniformVector3f (const std::string& uniformName, const Vector3f& uniformValue) = 0;
    virtual IShaderProgram& SetUniformVector4f (const std::string& uniformName, const Vector4f& uniformValue) = 0;

    // use program
    virtual void Use () const = 0;      
};