#ifndef _DEFAULT_FS_H_
#define _DEFAULT_FS_H_

#include <string>

std::string gDefaultFragmentShader = R"V0G0N(

	#version 330 core

	in vec3 fposition;
	in vec3 fnormal;

	out vec4 color;

	void main()
	{
		
		vec3 light_direction = normalize (vec3 (2.0, 5.0, 3.0) - fposition);
		float diff = max (dot (fnormal, light_direction), 0.0);
		float ambient = 0.0f;

		color = (ambient + diff) * vec4(1.0f, 0.0f, 0.0f, 1.0f);

	} 
	
)V0G0N";

#endif
