#ifndef _VECTOR3F_H_
#define _VECTOR3F_H_

#include <algorithm>

// Represents 3-dimensional float vector
class Vector3f
{
public:
	// constructors
	Vector3f () { };
	Vector3f (float x, float y, float z) : v { x, y, z } { };

	// copy constructor
	Vector3f (const Vector3f& other)
	{
		std::copy (other.v, other.v + 3, v);
	}

	// move constructor
	Vector3f (Vector3f&& other) : Vector3f ()
	{
		std::swap (v, other.v);
	}

	// assignment operator
	Vector3f& operator= (Vector3f other)
	{
		std::swap (v, other.v);
		return *this;
	}

	// destructor
	~Vector3f () { };

	// accessor and mutator
	float& operator[](unsigned int index);
	const float& operator[](unsigned int index) const;

	// operators
	Vector3f& operator*= (float f);
	Vector3f& operator+= (const Vector3f& other);
	Vector3f& operator-= (const Vector3f& other);
	friend inline Vector3f operator- (Vector3f v);

private:
	float v[3];
};

// mutator
inline float& Vector3f::operator[](unsigned int index)
{
	return v[index];
}

// accessor
inline const float& Vector3f::operator[](unsigned int index) const
{
	return v[index];
}

inline Vector3f& Vector3f::operator*=(float f)
{
	v[0] *= f;
	v[1] *= f;
	v[2] *= f;

	return *this;
}

inline Vector3f& Vector3f::operator+=(const Vector3f& other)
{
	v[0] += other.v[0];
	v[1] += other.v[1];
	v[2] += other.v[2];

	return *this;
}

inline Vector3f& Vector3f::operator-=(const Vector3f& other)
{
	v[0] -= other.v[0];
	v[1] -= other.v[1];
	v[2] -= other.v[2];

	return *this;
}

inline Vector3f operator* (Vector3f u, float f)
{
	u *= f;
	return u;
}

inline Vector3f operator* (float f, Vector3f u)
{
	u *= f;
	return u;
}

inline Vector3f operator+ (Vector3f u, const Vector3f& v)
{
	u += v;
	return u;
}

inline Vector3f operator- (Vector3f u, const Vector3f& v)
{
	u -= v;
	return u;
}

inline Vector3f operator- (Vector3f v)
{
	v.v[0] = -v.v[0];
	v.v[1] = -v.v[1];
	v.v[2] = -v.v[2];

	return v;
}

#endif

