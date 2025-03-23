#pragma once

#include "cilantroengine.h"
#include <string>
#include <memory>

namespace cilantro {

enum EBindingPoint { BP_MATRICES = 1, BP_POINTLIGHTS, BP_DIRECTIONALLIGHTS, BP_SPOTLIGHTS, BP_LIGHTVIEW_DIRECTIONAL };

struct IShader;
class Vector2f;
class Vector3f;
class Vector4f;
class Matrix3f;
class Matrix4f;

struct IShaderProgram 
{
    virtual ~IShaderProgram () {};

    // linking
    virtual void AttachShader (const std::shared_ptr<IShader> shader) = 0;  
    virtual void Link () = 0;

    // uniform manipulation
    virtual bool HasUniform (const std::string& uniformName) const = 0;
    virtual IShaderProgram& SetUniformFloat (const std::string& uniformName, float uniformValue) = 0;
    virtual IShaderProgram& SetUniformFloatv (const std::string& uniformName, const float* uniformValue, size_t count) = 0;
    virtual IShaderProgram& SetUniformVector2f (const std::string& uniformName, const Vector2f& uniformValue) = 0;
    virtual IShaderProgram& SetUniformVector3f (const std::string& uniformName, const Vector3f& uniformValue) = 0;
    virtual IShaderProgram& SetUniformVector4f (const std::string& uniformName, const Vector4f& uniformValue) = 0;
    virtual IShaderProgram& SetUniformMatrix3f (const std::string& uniformName, const Matrix3f& uniformValue) = 0;
    virtual IShaderProgram& SetUniformMatrix4f (const std::string& uniformName, const Matrix4f& uniformValue) = 0;
    virtual IShaderProgram& SetUniformMatrix3fv (const std::string& uniformName, const float* uniformValue, size_t count) = 0;
    virtual IShaderProgram& SetUniformMatrix4fv (const std::string& uniformName, const float* uniformValue, size_t count) = 0;

    // use program
    virtual void Use () const = 0;      
};

} // namespace cilantro