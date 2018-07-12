#include "cilantroengine.h"
#include "GLShader.h"
#include "GLShaderModel.h"
#include "LogMessage.h"
#include <iostream>

GLShaderModel::GLShaderModel ()
{
	shaderProgramId = glCreateProgram ();
}


GLShaderModel::~GLShaderModel ()
{
}

GLuint GLShaderModel::GetProgramId ()
{
	return shaderProgramId;
}


void GLShaderModel::AttachShader (GLShader shader)
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
		LogMessage (__FUNCTION__, EXIT_FAILURE) << "Unable to link shader" << shader.GetShaderId () << " to program" << shaderProgramId;
	}
}

void GLShaderModel::Use ()
{
	glUseProgram (shaderProgramId);
}
