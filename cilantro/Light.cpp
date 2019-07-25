#include "cilantroengine.h"
#include "GameObject.h"
#include "Light.h"

Light::Light ()
{
	isEnabled = false;
}

Light::~Light ()
{
}

void Light::SetEnabled (bool value)
{
	isEnabled = value;
}

bool Light::IsEnabled () const
{
	return isEnabled;
}