#include "cilantroengine.h"
#include "graphics/Renderer.h"
#include "graphics/GLShader.h"
#include "util/LogMessage.h"

#include <string>

GLShader::GLShader ()
{
}

GLShader::GLShader (std::string sourceCode, ShaderType type) : shaderType(type), shaderSourceCode(sourceCode)
{
	GLint success;
	char errorLog[512];
	const char * cSourceStr;

	switch (shaderType)
	{
		case ShaderType::VERTEX_SHADER:
			shaderId = glCreateShader (GL_VERTEX_SHADER);
			break;
		case ShaderType::FRAGMENT_SHADER:
			shaderId = glCreateShader (GL_FRAGMENT_SHADER);
			break;
	}

	cSourceStr = shaderSourceCode.c_str ();
	glShaderSource (shaderId, 1, &cSourceStr, NULL);
	glCompileShader (shaderId);

	glGetShaderiv (shaderId, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog (shaderId, 512, nullptr, errorLog);
		glDeleteShader (shaderId);
		LogMessage () << errorLog;
		LogMessage (__func__, EXIT_FAILURE) << "Unable to compile shader:" << shaderId << sourceCode;
	}
}

GLShader::~GLShader ()
{
}

GLuint GLShader::GetShaderId ()
{
	return shaderId;
}