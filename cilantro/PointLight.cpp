#include "PointLight.h"

PointLight::PointLight () : Light()
{
	lightColor = Vector3f (1.0f, 1.0f, 1.0f);
	ambiencePower = 0.0f;
	specularPower = 0.0f;
}

PointLight::~PointLight ()
{
}

void PointLight::Enable ()
{
	isEnabled = true;	
	InvokeCallbacks ("OnUpdatePointLight", this->GetHandle ());
}

void PointLight::Disable ()
{
	isEnabled = false;
	InvokeCallbacks ("OnUpdatePointLight", this->GetHandle ());
}

bool PointLight::IsEnabled () const
{
	return isEnabled;
}

PointLight& PointLight::SetLightColor (Vector3f color)
{
	lightColor = color;
	InvokeCallbacks ("OnUpdatePointLight", this->GetHandle ());
	return *this;
}

PointLight& PointLight::SetAmbiencePower (const float ambience)
{
	ambiencePower = ambience;
	InvokeCallbacks ("OnUpdatePointLight", this->GetHandle ());
	return *this;
}

PointLight & PointLight::SetSpecularPower (const float specular)
{
	specularPower = specular;
	InvokeCallbacks ("OnUpdatePointLight", this->GetHandle ());
	return *this;
}

Vector3f PointLight::GetLightColor () const
{
	return lightColor;
}

float PointLight::GetAmbiencePower () const
{
	return ambiencePower;
}

float PointLight::GetSpecularPower () const
{
	return specularPower;
}
