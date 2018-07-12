#ifndef _GLSHADERPROGRAM_H_
#define _GLSHADERPROGRAM_H_

class GLShaderModel
{
public:
	GLShaderModel ();
	~GLShaderModel ();

	// return program id
	GLuint GetProgramId ();

	// attach shader to a program
	void AttachShader (GLShader shader);

	// use model
	void Use ();

private:

	// ID of a shader program
	GLuint shaderProgramId;
};

#endif

