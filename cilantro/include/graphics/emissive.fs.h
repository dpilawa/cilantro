#ifndef _EMISSIVE_FS_H_
#define _EMISSIVE_FS_H_

#include <string>

#define xstr(a) str(a)
#define str(a) #a

std::string gEmissiveFragmentShader = 
R"(#version )"
xstr(CILANTRO_MIN_GL_VERSION)
R"V0G0N(

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