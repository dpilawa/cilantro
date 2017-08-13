#include "Light.h"

Light::Light () : lightPosition(0.0f, 0.0f, 0.0f, 1.0f)
{
	isEnabled = false;
}

Light::~Light ()
{
}

Vector4f Light::GetPosition ()
{
	return lightPosition * GetModelTransformMatrix ();
}
