#ifndef _BLINNPHONG_FS_H_
#define _BLINNPHONG_FS_H_

#include <string>

std::string gBlinnPhongFragmentShader = R"V0G0N(

	#version 140

	#define MAX_POINT_LIGHTS 64
	#define MAX_DIRECTIONAL_LIGHTS 16

	/* fragment position in world space */
	in vec3 fPosition;

	/* fragment normal */
	in vec3 fNormal;
	vec3 fNormal_N;

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

	struct DirectionalLightStruct
	{
		vec3 lightDirection;
		vec3 lightColor;
		float ambiencePower;
		float specularPower;
	};

	layout(std140) uniform UniformPointLightsBlock
	{
		int pointLightCount;
		PointLightStruct pointLights[MAX_POINT_LIGHTS];
	};

	layout(std140) uniform UniformDirectionalLightsBlock
	{
		int directionalLightCount;
		DirectionalLightStruct directionalLights[MAX_DIRECTIONAL_LIGHTS];
	};

	/* output color */
	out vec4 color;

	/* calculate diffuse coefficient */
	float CalculateDiffuse (float n_dot_l)
	{
		return clamp (n_dot_l, 0.0, 1.0);
	}

	float CalculateSpecular (vec3 lightDirection, float n_dot_l, float lightSpecularPower)
	{
		vec3 viewDirection = normalize (eyePosition - fPosition);
		vec3 halfwayDirection = normalize(lightDirection + viewDirection);
		return pow(clamp(dot(fNormal_N, halfwayDirection), 0.0, 1.0), fSpecularShininess) * lightSpecularPower * smoothstep(0.0, 0.5, n_dot_l);		
	}

	vec3 CalculateOutputColor (float ambientCoefficient, float diffuseCoefficient, float attenuationFactor, float specularCoefficient, vec3 lightColor)
	{
		return (ambientCoefficient * fAmbientColor + diffuseCoefficient * fDiffuseColor * attenuationFactor + specularCoefficient * fSpecularColor) * lightColor;
	}

	/* calculate contribution of one point light */
	vec4 CalculatePointLight (PointLightStruct light)
	{
		/* ambient component */
		float ambientCoefficient = light.ambiencePower;

		/* diffuse component */
		vec3 lightDirection = normalize (light.lightPosition - fPosition);
		float n_dot_l = dot (fNormal_N, lightDirection);
		float diffuseCoefficient = CalculateDiffuse (n_dot_l);
		
		/* specular component */
		float specularCoefficient = CalculateSpecular (lightDirection, n_dot_l, light.specularPower);
		
		/* attenuation */
		float distanceToLight = length (light.lightPosition - fPosition);
		float attenuationFactor = 1.0f / (light.attenuationConst + light.attenuationLinear * distanceToLight + light.attenuationQuadratic * distanceToLight * distanceToLight);

		/* aggregate output */
		return vec4 (CalculateOutputColor (ambientCoefficient, diffuseCoefficient, attenuationFactor, specularCoefficient, light.lightColor), 1.0);
	}

	/* calculate contribution of one directional light */
	vec4 CalculateDirectionalLight (DirectionalLightStruct light)
	{
		/* ambient component */
		float ambientCoefficient = light.ambiencePower;

		/* diffuse component */
		vec3 lightDirection = normalize (light.lightDirection);
		float n_dot_l = dot (fNormal_N, lightDirection);		
		float diffuseCoefficient = CalculateDiffuse (n_dot_l);

		/* specular component */
		float specularCoefficient = CalculateSpecular (lightDirection, n_dot_l, light.specularPower);

		/* attenuation */
		float attenuationFactor = 1.0f;

		/* aggregate output */
		return vec4 (CalculateOutputColor (ambientCoefficient, diffuseCoefficient, attenuationFactor, specularCoefficient, light.lightColor), 1.0);
	}

	void main()
	{
		color = vec4 (fEmissiveColor, 1.0);
		
		/* normalize normal again to fix interpolated normals */
		fNormal_N = normalize (fNormal);

		for (int i=0; i < pointLightCount; i++)
		{
			color += CalculatePointLight (pointLights[i]);
		}

		for (int i=0; i < directionalLightCount; i++)
		{
			color += CalculateDirectionalLight (directionalLights[i]);
		}
	} 
	
)V0G0N";

#endif
