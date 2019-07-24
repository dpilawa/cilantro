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

	/* material properties */
	uniform vec3 fAmbientColor;
	uniform vec3 fDiffuseColor;
	uniform vec3 fSpecularColor;
	uniform float fSpecularShininess;
	uniform vec3 fEmissiveColor;

	/* eye position in world space */
	uniform vec3 eyePosition;

	/* point lights data structures */

	struct PointLightStruct
	{
		vec3 lightPosition;	/* world space */
		vec3 lightColor;
		float ambiencePower;
		float specularPower;
		float attenuationConst;
		float attenuationLinear;
		float attenuationQuadratic;
	};

	layout(std140) uniform UniformPointLightsBlock 
	{
		int pointLightCount;
		PointLightStruct pointLights[MAX_LIGHTS];
	};

	/* output color */
	out vec4 color;

	void main()
	{
		color = vec4 (0.5 + 0.5 * fNormal, 1.0);
	} 
	
)V0G0N";

#endif
