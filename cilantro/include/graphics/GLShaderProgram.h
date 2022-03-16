#pragma once

#include "cilantroengine.h"
#include "glad/glad.h"
#include "graphics/ShaderProgram.h"

struct IShaderProgram;

class CGLShaderProgram : public CShaderProgram
{
public:
    __EAPI CGLShaderProgram ();
    __EAPI virtual ~CGLShaderProgram () {};

    ///////////////////////////////////////////////////////////////////////////

    __EAPI virtual void AttachShader (const IShader& shader) override;
    __EAPI virtual void Link () override;

    __EAPI virtual IShaderProgram& SetUniformFloat (const std::string& uniformName, float uniformValue) override;
    __EAPI virtual IShaderProgram& SetUniformVector2f (const std::string& uniformName, const Vector2f& uniformValue) override;
    __EAPI virtual IShaderProgram& SetUniformVector3f (const std::string& uniformName, const Vector3f& uniformValue) override;
    __EAPI virtual IShaderProgram& SetUniformVector4f (const std::string& uniformName, const Vector4f& uniformValue) override;
    
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


