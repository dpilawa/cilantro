#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>

enum class ShaderType { VERTEX_SHADER, FRAGMENT_SHADER };

class Shader
{
public:
	Shader ();
	virtual ~Shader () = 0;

protected:
	// shader type (vertex, fragment)
	ShaderType shaderType;

	// source code
	std::string shaderSourceCode;
};

#endif
