#ifndef _GLSHADER_H_
#define _GLSHADER_H_

#include "Renderer.h"
#include "LogMessage.h"
#include <string>
#include <iostream>
#include <GL/glew.h>
#include <string>

class GLShader
{
public:
	GLShader ();
	GLShader (std::string sourceCode, ShaderType type);
	~GLShader ();

	// return shader ID
	GLuint GetShaderId ();

private:

	// ID of a shader
	GLuint shaderId;

	// shader type (vertex, fragment)
	ShaderType shaderType;

	// source code
	std::string shaderSourceCode;
};

#endif
