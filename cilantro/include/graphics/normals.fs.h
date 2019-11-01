#ifndef _NORMALS_FS_H_
#define _NORMALS_FS_H_

#include <string>

std::string gNormalsFragmentShader = R"V0G0N(

	#version 140

	#define MAX_LIGHTS 100

	/* fragment position in world space */
	in vec3 fPosition;

	/* fragment normal */
	in vec3 fNormal;

	/* output color */
	out vec4 color;

	void main()
	{
		color = vec4 (0.5 + 0.5 * fNormal, 1.0);
	} 
	
)V0G0N";

#endif
