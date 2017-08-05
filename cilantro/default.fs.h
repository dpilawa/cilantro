#ifndef _DEFAULT_FS_H_
#define _DEFAULT_FS_H_

#include <string>

std::string gDefaultFragmentShader = R"V0G0N(

	#version 330 core

	in vec3 fPosition;
	in vec3 fNormal;
	in vec3 fDiffuseColor;

	out vec4 color;

	void main()
	{
		
		vec3 light_direction = normalize (vec3 (2.0, 4.0, 3.0) - fPosition);
		float diff = max (dot (fNormal, light_direction), 0.0);
		float ambient = 0.1f;

		color = (ambient + diff) * vec4(fDiffuseColor, 1.0f);

	} 
	
)V0G0N";

#endif
