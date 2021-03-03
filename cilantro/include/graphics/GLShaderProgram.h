#ifndef _GLSHADERPROGRAM_H_
#define _GLSHADERPROGRAM_H_

#include "glad/glad.h"
#include "graphics/ShaderProgram.h"
#include "graphics/GLShader.h"

class GLShaderProgram : public ShaderProgram
{
public:
    GLShaderProgram ();
    ~GLShaderProgram ();

    // return program id
    GLuint GetProgramId () const;

    // use program
    void Use ();

private:

    // attach shader to a program
    void LinkShader (const Shader& shader);

    // ID of a shader program
    GLuint shaderProgramId;
};

#endif

