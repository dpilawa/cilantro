#ifndef _VECTOR4F_H_
#define _VECTOR4F_H_

#include "Vector3f.h"
#include <algorithm>

// Represents 4-dimensional float vector
class Vector4f
{
public:
	// constructors
	Vector4f () : v { 0.0f, 0.0f, 0.0f, 0.0f } {};
	Vector4f (float x, float y, float z, float w) : v { x, y, z, w } {};
	Vector4f (float x, float y, float z) : v { x, y, z, 1.0f } {};
	Vector4f (const Vector3f& v3, float w) : v { v3[0], v3[1], v3[2], w } {};

	// copy constructor
	Vector4f (const Vector4f& other)
	{
		std::copy (other.v, other.v + 4, v);
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

	// accessor and mutator
	float& operator[] (unsigned int index);
	const float& operator[] (unsigned int index) const;

private:
	float v[4];

};

// mutator
inline float& Vector4f::operator[] (unsigned int index)
{
	return v[index];
}

// accessor
inline const float& Vector4f::operator[] (unsigned int index) const
{
	return v[index];
}

#endif

