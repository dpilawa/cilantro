#ifndef _DEFAULT_VS_H_
#define _DEFAULT_VS_H_

#include <string>

std::string gDefaultVertexShader = R"V0G0N(

	#version 330 core

	uniform mat4 mModel;
	uniform mat4 mView;
	uniform mat4 mProjection;

	layout (location = 0) in vec3 vposition;
	layout (location = 1) in vec3 vnormal;

	out vec3 fposition;
	out vec3 fnormal;

	void main ()
	{
		gl_Position = mProjection * mView * mModel * vec4 (vposition.xyz, 1.0);
		
		fposition = vec3 (mModel * vec4 (vposition.xyz, 1.0));
		fnormal = normalize (mModel * vec4 (vnormal, 1.0));
		
	}
	
)V0G0N";

#endif
