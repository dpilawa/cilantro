#include "cilantroengine.h"
#include "graphics/GLShader.h"
#include "graphics/GLShaderProgram.h"
#include "util/LogMessage.h"

#include <iostream>

GLShaderProgram::GLShaderProgram ()
{
	shaderProgramId = glCreateProgram ();
}


GLShaderProgram::~GLShaderProgram ()
{
}

GLuint GLShaderProgram::GetProgramId ()
{
	return shaderProgramId;
}


void GLShaderProgram::AttachShader (GLShader shader)
{
	GLint success;
	char errorLog[512];

	glAttachShader (shaderProgramId, shader.GetShaderId ());
	glLinkProgram (shaderProgramId);

	glGetProgramiv (shaderProgramId, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog (shaderProgramId, 512, nullptr, errorLog);
		glDeleteProgram (shaderProgramId);
		LogMessage () << errorLog;
		LogMessage (__func__, EXIT_FAILURE) << "Unable to link shader" << shader.GetShaderId () << " to program" << shaderProgramId;
	}
}

void GLShaderProgram::Use ()
{
	glUseProgram (shaderProgramId);
}
