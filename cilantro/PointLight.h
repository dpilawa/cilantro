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

	// getters
	Vector3f GetLightColor () const;
	float GetAmbiencePower () const;
	float GetSpecularPower () const;

private:
	Vector3f lightColor;
	float ambiencePower;
	float specularPower;
	float attenuation;
};

#endif