#ifndef _VECTOR4F_H_
#define _VECTOR4F_H_

#include "Matrix4f.h"
#include <algorithm>

// Represents 4-dimensional float vector
class Vector4f
{
	friend class Matrix4f;
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
	float& operator[](unsigned int index);
	const float& operator[](unsigned int index) const;

	// multiply by matrix
	Vector4f& operator*= (const Matrix4f& m);

private:
	float v[4];

};

// mutator
inline float& Vector4f::operator[](unsigned int index)
{
	return v[index];
}

// accessor
inline const float& Vector4f::operator[](unsigned int index) const
{
	return v[index];
}

// compound assignment operator for vector by matrix multiplication 
inline Vector4f & Vector4f::operator*=(const Matrix4f & m)
{
	Vector4f temp (*this);

	v[0] = temp.v[0] * m[0][0] + temp.v[1] * m[0][1] + temp.v[2] * m[0][2] + temp.v[3] * m[0][3];
	v[1] = temp.v[0] * m[1][0] + temp.v[1] * m[1][1] + temp.v[2] * m[1][2] + temp.v[3] * m[1][3];
	v[2] = temp.v[0] * m[2][0] + temp.v[1] * m[2][1] + temp.v[2] * m[2][2] + temp.v[3] * m[2][3];
	v[3] = temp.v[0] * m[3][0] + temp.v[1] * m[3][1] + temp.v[2] * m[3][2] + temp.v[3] * m[3][3];

	return *this;
}

inline Vector4f operator* (Vector4f v, const Matrix4f& m)
{
	v *= m;
	return v;
};

#endif

