#ifndef _PHONG_FS_H_
#define _PHONG_FS_H_

#include <string>

std::string gPhongFragmentShader = R"V0G0N(

	#version 330 core

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

	layout (std140) uniform UniformPointLightsBlock 
	{
		int pointLightCount;
		PointLightStruct pointLights[MAX_LIGHTS];
	};

	/* output color */
	out vec4 color;

	/* calculate contribution of one point light */
	vec4 CalculatePointLight (PointLightStruct light)
	{
		/* normalize again to account for interpolated normals */
		vec3 fNormal_N = normalize (fNormal);

		/* ambient component */
		float ambientCoefficient = light.ambiencePower;

		/* diffuse component */
		vec3 lightDirection = normalize (light.lightPosition - fPosition);
		float n_dot_l = dot (fNormal_N, lightDirection);
		float diffuseCoefficient = clamp (n_dot_l, 0.0, 1.0);

		/* specular component */
		vec3 viewDirection = normalize (eyePosition - fPosition);
		vec3 reflectionDirection = reflect (-lightDirection, fNormal_N);
		float specularCoefficient = pow(clamp(dot(viewDirection, reflectionDirection), 0.0, 1.0), fSpecularShininess) * light.specularPower * smoothstep(0.0, 0.15, n_dot_l);

		/* attenuation */
		float distanceToLight = length (light.lightPosition - fPosition);
		float attenuationFactor = 1.0 / (light.attenuationConst + light.attenuationLinear * distanceToLight + light.attenuationQuadratic * distanceToLight * distanceToLight);

		/* aggregate output */
		vec3 outputColor = (ambientCoefficient * fAmbientColor + diffuseCoefficient * fDiffuseColor * attenuationFactor + specularCoefficient * fSpecularColor) * light.lightColor;
		return vec4 (outputColor, 1.0);
	}

	void main()
	{
		color = vec4 (fEmissiveColor, 1.0);

		for (int i=0; i < pointLightCount; i++)
		{
			color += CalculatePointLight (pointLights[i]);
		} 
	} 
	
)V0G0N";

#endif
