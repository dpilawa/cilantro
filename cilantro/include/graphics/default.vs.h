#ifndef _DEFAULT_VS_H_
#define _DEFAULT_VS_H_

#include <string>

#define xstr(a) str(a)
#define str(a) #a

std::string gDefaultVertexShader = 
R"(#version )"
xstr(CILANTRO_MIN_GL_VERSION)
R"V0G0N(

	/* transformation matrices */
	uniform mat4 mModel;
	uniform mat3 mNormal;

	layout(std140) uniform UniformMatricesBlock
	{
		mat4 mView;
		mat4 mProjection;
	};
)V0G0N"
#if (CILANTRO_MIN_GL_VERSION >= 330)
R"V0G0N(
	layout (location = 0) in vec3 vPosition;
	layout (location = 1) in vec3 vNormal;
)V0G0N"
#else
R"V0G0N(
	in vec3 vPosition;
	in vec3 vNormal;
)V0G0N"
#endif	
R"V0G0N(
	out vec3 fPosition;
	out vec3 fNormal;
	out vec3 fNormal_View;

	void main ()
	{
		gl_Position = mProjection * mView * mModel * vec4 (vPosition.xyz, 1.0);
		
		fPosition = vec3 (mModel * vec4 (vPosition.xyz, 1.0));
		fNormal = normalize (mNormal * vNormal);
	}
	
)V0G0N";

#endif
