#ifndef _POINTLIGHT_H_
#define _POINTLIGHT_H_

#include "cilantroengine.h"
#include "Light.h"
#include "Vector3f.h"

class PointLight : public Light
{
public:
	__EAPI PointLight ();
	__EAPI ~PointLight ();

	__EAPI void SetEnabled (bool value);

	// set color of the light
	__EAPI PointLight& SetLightColor (const Vector3f color);

	// set lighting powers
	__EAPI PointLight& SetAmbiencePower (const float ambience);
	__EAPI PointLight& SetSpecularPower (const float specular);

	// set attenuation factor
	__EAPI PointLight& SetConstantAttenuationFactor (const float attenuation);
	__EAPI PointLight& SetLinearAttenuationFactor (const float attenuation);
	__EAPI PointLight& SetQuadraticAttenuationFactor (const float attenuation);

	// getters
	__EAPI Vector3f GetColor () const;
	__EAPI float GetAmbiencePower () const;
	__EAPI float GetSpecularPower () const;
	__EAPI float GetConstantAttenuationFactor () const;
	__EAPI float GetLinearAttenuationFactor () const;
	__EAPI float GetQuadraticAttenuationFactor () const;

private:

	Vector3f lightColor;
	float ambiencePower;
	float specularPower;
	float attenuationConst;
	float attenuationLinear;
	float attenuationQuadratic;
};

#endif