#ifndef _GLSHADERPROGRAM_H_
#define _GLSHADERPROGRAM_H_

#include "GLShader.h"
#include "LogMessage.h"

class GLShaderModel
{
public:
	GLShaderModel ();
	~GLShaderModel ();

	// attach shader to a program
	void AttachShader (GLShader shader);

	// use model
	void Use ();

private:

	// ID of a shader program
	GLuint shaderProgramId;
};

#endif

