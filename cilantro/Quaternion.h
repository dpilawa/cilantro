#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "Vector3f.h"

class Quaternion
{
public:
	// constructors
	Quaternion () { };
	Quaternion (float a, float b, float c, float d) : s (a), v {b, c, d} { };
	Quaternion (float scalar, Vector3f vector) : s (scalar), v (vector) { };

	// copy constructor
	Quaternion (const Quaternion& other)
	{
		s = other.s;
		v = other.v;
	}

	// move constructor
	Quaternion (Quaternion&& other) : Quaternion ()
	{
		std::swap (s, other.s);
		v = std::move (other.v);
	}

	// assignment operator
	Quaternion& operator= (Quaternion other)
	{
		std::swap (s, other.s);
		v = std::move (other.v);
		return *this;
	}

	// destructor
	~Quaternion () { };

	// operators
	Quaternion& operator*= (float f);
	Quaternion& operator+= (const Quaternion& other);
	Quaternion& operator-= (const Quaternion& other);

	friend class Mathf;

private:
	float s;
	Vector3f v;
};

inline Quaternion& Quaternion::operator*=(float f)
{
	s *= f;
	v *= f;

	return *this;
};

inline Quaternion& Quaternion::operator+=(const Quaternion& other)
{
	s += other.s;
	v += other.v;

	return *this;
};

inline Quaternion& Quaternion::operator-=(const Quaternion& other)
{
	s -= other.s;
	v -= other.v;

	return *this;
};

inline Quaternion operator* (Quaternion q, float f)
{
	q *= f;
	return q;
};

inline Quaternion operator* (float f, Quaternion q)
{
	q *= f;
	return q;
};

inline Quaternion operator+ (Quaternion q, const Quaternion& r)
{
	q += r;
	return q;
};

inline Quaternion operator- (Quaternion q, const Quaternion& r)
{
	q -= r;
	return q;
};

#endif

