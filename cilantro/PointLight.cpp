#include "PointLight.h"

PointLight::PointLight () : Light()
{
	lightColor = Vector3f (1.0f, 1.0f, 1.0f);
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

void PointLight::SetLightColor (Vector3f color)
{
	lightColor = color;
	InvokeCallbacks ("OnUpdatePointLight", this->GetHandle ());
}

Vector3f PointLight::GetLightColor () const
{
	return lightColor;
}

