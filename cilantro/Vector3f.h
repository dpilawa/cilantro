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

	// getters
	float GetX () const;
	float GetY () const;
	float GetZ () const;
	float* GetDataPointer ();

	// returns vector length
	float Length ();

	// normalize vector
	Vector3f& Normalize ();

	// cross product of two vectors
	friend inline Vector3f Cross (const Vector3f& v1, const Vector3f& v2);

	// operators
	Vector3f& operator+= (const Vector3f& m);
	Vector3f& operator-= (const Vector3f& m);
	friend inline Vector3f operator- (Vector3f v);

private:
	float v[3];
};


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

inline Vector3f operator+ (Vector3f u, const Vector3f& v)
{
	u += v;
	return u;
};

inline Vector3f operator- (Vector3f u, const Vector3f& v)
{
	u -= v;
	return u;
};

inline Vector3f Cross (const Vector3f& v1, const Vector3f& v2)
{
	return Vector3f (v1.v[1] * v2.v[2] - v1.v[2] * v2.v[1], v1.v[2] * v2.v[0] - v1.v[0] * v2.v[2], v1.v[0] * v2.v[1] - v1.v[1] * v2.v[0]);
}

inline Vector3f operator- (Vector3f v)
{
	v.v[0] = -v.v[0];
	v.v[1] = -v.v[1];
	v.v[2] = -v.v[2];

	return v;
}

#endif

