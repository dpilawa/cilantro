#include "Vector3f.h"

Vector3f& Vector3f::Normalize ()
{
	float lengthDiv = 1.0f / std::sqrtf (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

	v[0] *= lengthDiv;
	v[1] *= lengthDiv;
	v[2] *= lengthDiv;

	return *this;
}


