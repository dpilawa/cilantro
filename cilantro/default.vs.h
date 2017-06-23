#ifndef _DEFAULT_VS_H_
#define _DEFAULT_VS_H_

#include <string>

std::string gDefaultVertexShader = R"V0G0N(

	#version 330 core
	layout (location = 0) in vec3 position;

	void main ()
	{
		gl_Position = vec4 (position.x, position.y, position.z, 1.0);
	}
	
)V0G0N";

#endif
