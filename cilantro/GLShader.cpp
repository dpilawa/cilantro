#include "GLShader.h"

GLShader::GLShader ()
{
}

GLShader::GLShader (std::string sourceCode, ShaderType type) : shaderSourceCode(sourceCode), shaderType(type)
{
	GLint success;
	const char * cStr;

	switch (type)
	{
	case ShaderType::VERTEX_SHADER:
		shaderId = glCreateShader (GL_VERTEX_SHADER);
		break;
	case ShaderType::FRAGMENT_SHADER:
		shaderId = glCreateShader (GL_FRAGMENT_SHADER);
		break;
	}

	cStr = shaderSourceCode.c_str ();
	glShaderSource (shaderId, 1, &cStr, NULL);
	glCompileShader (shaderId);

	glGetShaderiv (shaderId, GL_COMPILE_STATUS, &success);
	
	if (!success)
	{
		LogMessage (__FUNCTION__, EXIT_FAILURE) << "Unable to compile shader " << shaderId;
	}
}

GLShader::~GLShader ()
{
}

GLuint GLShader::GetShaderId ()
{
	return shaderId;
}
