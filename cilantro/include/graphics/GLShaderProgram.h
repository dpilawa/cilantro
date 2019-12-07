#ifndef _GLSHADERPROGRAM_H_
#define _GLSHADERPROGRAM_H_

#include "glad/glad.h"
#include "graphics/GLShader.h"

class GLShaderProgram
{
public:
	GLShaderProgram ();
	~GLShaderProgram ();

	// return program id
	GLuint GetProgramId ();

	// attach shader to a program
	void AttachShader (GLShader shader);

	// use program
	void Use ();

private:

	// ID of a shader program
	GLuint shaderProgramId;
};

#endif

