#ifndef _EMISSIVE_FS_H_
#define _EMISSIVE_FS_H_

#include <string>

std::string gEmissiveFragmentShader = R"V0G0N(

	#version 140

	/* material properties */
	uniform vec3 fEmissiveColor;

	/* output color */
	out vec4 color;

	void main()
	{
		color = vec4 (fEmissiveColor, 1.0);
	} 
	
)V0G0N";

#endif