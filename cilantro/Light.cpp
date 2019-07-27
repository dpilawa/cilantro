#include "cilantroengine.h"
#include "GameObject.h"
#include "Light.h"

Light::Light ()
{
	isLight = true;
	isEnabled = false;
	
	lightColor = Vector3f (1.0f, 1.0f, 1.0f);
	
	ambiencePower = 0.0f;
	specularPower = 0.0f;
}

Light::~Light ()
{
}

void Light::SetEnabled (bool value)
{
	isEnabled = value;
	InvokeOnUpdateCallbacks ();
}

bool Light::IsEnabled () const
{
	return isEnabled;
}

Light& Light::SetColor (Vector3f color)
{
	lightColor = color;
	InvokeOnUpdateCallbacks ();
	return *this;
}

Light& Light::SetAmbiencePower (const float ambience)
{
	ambiencePower = ambience;
	InvokeOnUpdateCallbacks ();
	return *this;
}

Light & Light::SetSpecularPower (const float specular)
{
	specularPower = specular;
	InvokeOnUpdateCallbacks ();
	return *this;
}

Vector3f Light::GetColor () const
{
	if (isEnabled)
	{
		return lightColor;
	}
	else
	{
		return Vector3f (0.0f, 0.0f, 0.0f);	
	}
}

float Light::GetAmbiencePower () const
{
	if (isEnabled)
	{
		return ambiencePower;
	}
	else 
	{
		return 0.0f;
	}
}

float Light::GetSpecularPower () const
{
	if (isEnabled)
	{
		return specularPower;
	}
	else
	{
		return 0.0f;
	}
}