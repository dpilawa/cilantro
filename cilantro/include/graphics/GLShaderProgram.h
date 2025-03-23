#pragma once

#include "cilantroengine.h"
#include "glad/gl.h"
#include "graphics/ShaderProgram.h"

namespace cilantro {

struct IShaderProgram;

class __CEAPI GLShaderProgram : public ShaderProgram
{
public:
    __EAPI GLShaderProgram ();
    __EAPI virtual ~GLShaderProgram () {};

    ///////////////////////////////////////////////////////////////////////////

    __EAPI virtual void AttachShader (const std::shared_ptr<IShader> shader) override;
    __EAPI virtual void Link () override;

    __EAPI virtual bool HasUniform (const std::string& uniformName) const override;
    __EAPI virtual IShaderProgram& SetUniformFloat (const std::string& uniformName, float uniformValue) override;
    __EAPI virtual IShaderProgram& SetUniformFloatv (const std::string& uniformName, const float* uniformValue, size_t count) override;
    __EAPI virtual IShaderProgram& SetUniformVector2f (const std::string& uniformName, const Vector2f& uniformValue) override;
    __EAPI virtual IShaderProgram& SetUniformVector3f (const std::string& uniformName, const Vector3f& uniformValue) override;
    __EAPI virtual IShaderProgram& SetUniformVector4f (const std::string& uniformName, const Vector4f& uniformValue) override;
    __EAPI virtual IShaderProgram& SetUniformMatrix3f (const std::string& uniformName, const Matrix3f& uniformValue) override;
    __EAPI virtual IShaderProgram& SetUniformMatrix4f (const std::string& uniformName, const Matrix4f& uniformValue) override;
    __EAPI virtual IShaderProgram& SetUniformMatrix3fv (const std::string& uniformName, const float* uniformValue, size_t count) override;
    __EAPI virtual IShaderProgram& SetUniformMatrix4fv (const std::string& uniformName, const float* uniformValue, size_t count) override;    

    __EAPI void Use () const override;

    ///////////////////////////////////////////////////////////////////////////

    // return GL ids
    GLuint GetProgramId () const;
    GLuint GetUniformLocationId (const std::string& uniformName) const;

    // bind uniform block
    void BindUniformBlock (const std::string& blockName, EBindingPoint bp);

private:
    // ID of a shader program
    GLuint m_glShaderProgramId;
};

} // namespace cilantro


