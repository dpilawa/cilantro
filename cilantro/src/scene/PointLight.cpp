#include "cilantroengine.h"
#include "scene/PointLight.h"
#include "scene/Light.h"
#include "math/Vector3f.h"
#include "graphics/Renderer.h"
#include "util/CallbackProvider.h"

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

void PointLight::OnUpdate (Renderer& renderer)
{
	Light::OnUpdate (renderer);
	renderer.Update (*this);
}