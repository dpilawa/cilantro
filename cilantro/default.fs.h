#ifndef _DEFAULT_FS_H_
#define _DEFAULT_FS_H_

#include <string>

std::string gDefaultFragmentShader = R"V0G0N(

	#version 330 core
	out vec4 color;

	void main()
	{
		color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	} 
	
)V0G0N";

#endif
