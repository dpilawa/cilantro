#include "cilantroengine.h"
#include "PointLight.h"
#include "Light.h"
#include "GameObject.h"
#include "Vector3f.h"
#include "CallbackProvider.h"

PointLight::PointLight () : Light()
{
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

void PointLight::SetEnabled (bool value)
{
	Light::SetEnabled (value);	
	InvokeCallbacks ("OnUpdatePointLight", this->GetHandle ());
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
