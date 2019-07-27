#include "cilantroengine.h"
#include "PointLight.h"
#include "Light.h"
#include "GameObject.h"
#include "Vector3f.h"
#include "CallbackProvider.h"

PointLight::PointLight () : Light()
{
	attenuationConst = 1.0f;
	attenuationLinear = 0.0f;
	attenuationQuadratic = 0.0f;
}

PointLight::~PointLight ()
{
}

PointLight& PointLight::SetConstantAttenuationFactor (const float attenuation)
{
	attenuationConst = attenuation;
	InvokeOnUpdateCallbacks ();
	return *this;
}

PointLight& PointLight::SetLinearAttenuationFactor (const float attenuation)
{
	attenuationLinear = attenuation;
	InvokeOnUpdateCallbacks ();
	return *this;
}

PointLight& PointLight::SetQuadraticAttenuationFactor (const float attenuation)
{
	attenuationQuadratic = attenuation;
	InvokeOnUpdateCallbacks ();
	return *this;
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

void PointLight::InvokeOnUpdateCallbacks ()
{
	InvokeCallbacks ("OnUpdatePointLight", this->GetHandle ());
}