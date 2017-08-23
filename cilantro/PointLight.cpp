#include "PointLight.h"

PointLight::PointLight () : Light()
{
	lightPosition = Vector4f (0.0f, 0.0f, 0.0f, 1.0f);
	lightColor = Vector3f (1.0f, 1.0f, 1.0f);
	
	ambiencePower = 0.0f;
	specularPower = 0.0f;

	attenuationConst = 1.0f;
	attenuationLinear = 0.0f;
	attenuationQuadratic = 0.0f;
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

PointLight& PointLight::SetConstantAttenuationFactor (const float attenuation)
{
	attenuationConst = attenuation;
	InvokeCallbacks ("OnUpdatePointLight", this->GetHandle ());
	return *this;
}

PointLight& PointLight::SetLinearAttenuationFactor (const float attenuation)
{
	attenuationLinear = attenuation;
	InvokeCallbacks ("OnUpdatePointLight", this->GetHandle ());
	return *this;
}

PointLight& PointLight::SetQuadraticAttenuationFactor (const float attenuation)
{
	attenuationQuadratic = attenuation;
	InvokeCallbacks ("OnUpdatePointLight", this->GetHandle ());
	return *this;
}

Vector3f PointLight::GetColor () const
{
	return lightColor;
}

Vector4f PointLight::GetPosition ()
{
	return lightPosition * GetModelTransformMatrix ();
}

float PointLight::GetAmbiencePower () const
{
	return ambiencePower;
}

float PointLight::GetSpecularPower () const
{
	return specularPower;
}

float PointLight::GetConstantAttenuationFactor () const
{
	return attenuationConst;
}

float PointLight::GetLinearAttenuationFactor () const
{
	return attenuationLinear;
}

float PointLight::GetQuadraticAttenuationFactor () const
{
	return attenuationQuadratic;
}
