#ifndef _DEFAULT_FS_H_
#define _DEFAULT_FS_H_

#include <string>

std::string gDefaultFragmentShader = R"V0G0N(

	#version 330 core

	#define MAX_LIGHTS 100

	in vec3 fPosition;
	in vec3 fNormal;
	in vec3 fDiffuseColor;

	struct PointLightStruct
	{
		vec3 lightColor;
		vec3 lightPosition;
	};

	layout (std140) uniform UniformPointLightsBlock 
	{
		int pointLightCount;
		PointLightStruct pointLights[MAX_LIGHTS];
	};

	out vec4 color;

	vec3 CalculatePointLight (PointLightStruct light)
	{
		vec3 lightDirection = normalize (light.lightPosition.xyz - fPosition);
		float diffuseCoefficient = max (dot (fNormal, lightDirection), 0.0);
		
		vec3 diffuse = diffuseCoefficient * fDiffuseColor * light.lightColor.xyz;

		return diffuse;
	}

	void main()
	{
		for (int i=0; i < pointLightCount; i++)
		{
			color += CalculatePointLight (pointLights[i]);
		}
	} 
	
)V0G0N";

#endif
