#ifndef _FLATQUAD_FS_H_
#define _FLATQUAD_FS_H_

#include <string>

#define xstr(a) str(a)
#define str(a) #a

std::string gFlatQuadFragmentShader = 
R"(#version )"
xstr(CILANTRO_MIN_GL_VERSION)
R"V0G0N(

	/* texture coords */
	in vec2 fTextureCoordinates;

	/* output color */
	out vec4 color;

	/* texture */
	uniform sampler2D fScreenTexture;

	void main()
	{
		color = texture (fScreenTexture, fTextureCoordinates);
	} 
	
)V0G0N";

#endif
