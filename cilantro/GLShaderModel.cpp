#include "GLShaderModel.h"

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

	glAttachShader (shaderProgramId, shader.GetShaderId ());
	glLinkProgram (shaderProgramId);

	glGetProgramiv (shaderProgramId, GL_LINK_STATUS, &success);

	if (!success)
	{
		LogMessage (__FUNCTION__, EXIT_FAILURE) << "Unable to attach shader " << shader.GetShaderId () << " to program " << shaderProgramId;
	}
}

void GLShaderModel::Use ()
{
	glUseProgram (shaderProgramId);
}
