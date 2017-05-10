#ifndef _VECTOR4F_H_
#define _VECTOR4F_H_

#include <algorithm>

#define V(n) v [n - 1]

// Represents 4-dimensional float vector
class Vector4f
{
	float v[4];
public:
	// constructors
	Vector4f () {};
	Vector4f (float x, float y, float z, float w) : v { x, y, z, w } {};
	Vector4f (float x, float y, float z) : v { x, y, z, 1.0f } {};

	// copy constructor
	Vector4f (const Vector4f& other)
	{
		std::copy (other.v, other.v + 3, v);
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

	// other operators
};

#endif

