#ifndef _DEFAULT_VS_H_
#define _DEFAULT_VS_H_

#include <string>

std::string gDefaultVertexShader = R"V0G0N(

	#version 330 core

	uniform mat4 mModel;
	uniform mat3 mNormal;

	uniform UniformBlock 
	{
		mat4 mView;
		mat4 mProjection;
	};

	layout (location = 0) in vec3 vPosition;
	layout (location = 1) in vec3 vNormal;

	out vec3 fPosition;
	out vec3 fNormal;

	void main ()
	{
		gl_Position = mProjection * mView * mModel * vec4 (vPosition.xyz, 1.0);
		
		fPosition = vec3 (mModel * vec4 (vPosition.xyz, 1.0));
		fNormal = normalize (mNormal * vNormal);
	}
	
)V0G0N";

#endif
