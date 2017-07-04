#ifndef _VECTOR4F_H_
#define _VECTOR4F_H_

#include <algorithm>
#include "Vector3f.h"

// Represents 4-dimensional float vector
class Vector4f
{
public:
	// constructors
	Vector4f () {};
	Vector4f (float x, float y, float z, float w) : v { x, y, z, w } {};
	Vector4f (float x, float y, float z) : v { x, y, z, 1.0f } {};

	// copy constructor
	Vector4f (const Vector4f& other)
	{
		std::copy (other.v, other.v + 4, v);
	}

	// copy constructor (from 3-dimensional vector)
	Vector4f (const Vector3f& other)
	{
		std::copy (other.v, other.v + 3, v);
		v[3] = 1.0f;
	}

	// move constructor
	Vector4f (Vector4f&& other) : Vector4f()
	{
		std::swap (v, other.v);
	}

	// assignment operator
	Vector4f& operator= (Vector4f other)
	{
		std::swap (v, other.v);
		return *this;
	}

	// destructor
	~Vector4f () { };

private:
	float v[4];

};

#endif

