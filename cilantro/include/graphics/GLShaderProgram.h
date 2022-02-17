#ifndef _GLSHADERPROGRAM_H_
#define _GLSHADERPROGRAM_H_

#include "glad/glad.h"
#include "graphics/ShaderProgram.h"
#include "graphics/GLShader.h"
#include <string>

enum BindingPoint { BP_MATRICES = 1, BP_POINTLIGHTS, BP_DIRECTIONALLIGHTS, BP_SPOTLIGHTS };

class GLShaderProgram : public ShaderProgram
{
public:
    __EAPI GLShaderProgram ();
    __EAPI virtual ~GLShaderProgram ();

    // return GL ids
    GLuint GetProgramId () const;
    GLuint GetUniformLocationId (const std::string& uniformName) const;

    // bind uniform block
    void BindUniformBlock (const std::string& blockName, BindingPoint bp);

    // uniform manipulation
    __EAPI virtual ShaderProgram& SetUniformFloat (const std::string& uniformName, float uniformValue);
    __EAPI virtual ShaderProgram& SetUniformVector2f (const std::string& uniformName, const Vector2f& uniformValue);
    __EAPI virtual ShaderProgram& SetUniformVector3f (const std::string& uniformName, const Vector3f& uniformValue);
    __EAPI virtual ShaderProgram& SetUniformVector4f (const std::string& uniformName, const Vector4f& uniformValue);
    
    // link program
    void Link ();

    // use program
    void Use () const;

private:

    // attach shader to a program
    void AttachShader (const Shader& shader);

    // ID of a shader program
    GLuint shaderProgramId;
};

#endif

