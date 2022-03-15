#pragma once

#include "cilantroengine.h"
#include "glad/glad.h"
#include "graphics/ShaderProgram.h"

enum EBindingPoint { BP_MATRICES = 1, BP_POINTLIGHTS, BP_DIRECTIONALLIGHTS, BP_SPOTLIGHTS };

class CGLShaderProgram : public CShaderProgram
{
public:
    __EAPI CGLShaderProgram ();
    __EAPI virtual ~CGLShaderProgram ();

    // return GL ids
    GLuint GetProgramId () const;
    GLuint GetUniformLocationId (const std::string& uniformName) const;

    // bind uniform block
    void BindUniformBlock (const std::string& blockName, EBindingPoint bp);

    // uniform manipulation
    __EAPI virtual CShaderProgram& SetUniformFloat (const std::string& uniformName, float uniformValue);
    __EAPI virtual CShaderProgram& SetUniformVector2f (const std::string& uniformName, const Vector2f& uniformValue);
    __EAPI virtual CShaderProgram& SetUniformVector3f (const std::string& uniformName, const Vector3f& uniformValue);
    __EAPI virtual CShaderProgram& SetUniformVector4f (const std::string& uniformName, const Vector4f& uniformValue);
    
    // link program
    void Link ();

    // use program
    void Use () const;

private:

    // attach shader to a program
    void AttachShader (const CShader& shader);

    // ID of a shader program
    GLuint m_glShaderProgramId;
};


