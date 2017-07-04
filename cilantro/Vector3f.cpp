#include "Vector3f.h"

float Vector3f::GetX ()
{
	return v[0];
}

float Vector3f::GetY ()
{
	return v[1];
}

float Vector3f::GetZ ()
{
	return v[2];
}

float Vector3f::Length ()
{
	return std::sqrtf (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void Vector3f::Normalize ()
{
	float lengthDiv = 1.0f / Length ();

	v[0] *= lengthDiv;
	v[1] *= lengthDiv;
	v[2] *= lengthDiv;
}


