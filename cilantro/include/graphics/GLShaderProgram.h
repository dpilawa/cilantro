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
    GLShaderProgram ();
    ~GLShaderProgram ();

    // return program id
    GLuint GetProgramId () const;

    // bind uniform block
    void BindUniformBlock (const std::string& blockName, BindingPoint bp);

    // link program
    void Link ();

    // use program
    void Use ();

private:

    // attach shader to a program
    void AttachShader (const Shader& shader);

    // ID of a shader program
    GLuint shaderProgramId;
};

#endif

