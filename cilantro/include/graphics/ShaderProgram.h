#ifndef _SHADERPROGRAM_H_
#define _SHADERPROGRAM_H_

#include "cilantroengine.h"
#include "graphics/Shader.h"

class ShaderProgram
{
public:
	ShaderProgram ();
	virtual ~ShaderProgram ();

	// attach shader to a program
	virtual void LinkShader (Shader& shader) = 0;

	// use program
	virtual void Use () = 0;

};

#endif