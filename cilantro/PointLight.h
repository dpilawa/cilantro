#ifndef _POINTLIGHT_H_
#define _POINTLIGHT_H_

#include "Light.h"
#include "Vector3f.h"

class PointLight : public Light
{
public:
	PointLight ();
	~PointLight ();

	void Enable ();
	void Disable ();

	bool IsEnabled () const;

	// set color of the light
	PointLight& SetLightColor (const Vector3f color);

	// set lighting powers
	PointLight& SetAmbiencePower (const float ambience);
	PointLight& SetSpecularPower (const float specular);

	// set attenuation factor
	PointLight& SetConstantAttenuationFactor (const float attenuation);
	PointLight& SetLinearAttenuationFactor (const float attenuation);
	PointLight& SetQuadraticAttenuationFactor (const float attenuation);

	// getters
	Vector4f GetPosition ();
	Vector3f GetColor () const;
	float GetAmbiencePower () const;
	float GetSpecularPower () const;
	float GetConstantAttenuationFactor () const;
	float GetLinearAttenuationFactor () const;
	float GetQuadraticAttenuationFactor () const;

private:
	Vector4f lightPosition;
	Vector3f lightColor;
	float ambiencePower;
	float specularPower;
	float attenuationConst;
	float attenuationLinear;
	float attenuationQuadratic;
};

#endif