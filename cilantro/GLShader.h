#ifndef _GLSHADER_H_
#define _GLSHADER_H_

#include "glad/glad.h"
#include "Renderer.h"
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
