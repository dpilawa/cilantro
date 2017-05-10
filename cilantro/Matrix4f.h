#ifndef _MATRIX4F_H_
#define _MATRIX4F_H_

#include <algorithm>

class Matrix4f
{
	float m[4][4];
public:
	// constructor
	Matrix4f () { };
	
	// copy constructor
	Matrix4f (const Matrix4f& other)
	{
		std::copy (other.m, other.m + 15, m);
	}

	// move constructor
	Matrix4f (Matrix4f&& other) : Matrix4f ()
	{
		std::swap (m, other.m);
	}

	// assignment operator
	Matrix4f& operator= (Matrix4f other)
	{
		std::swap (m, other.m);
		return *this;
	}

	// destructor
	~Matrix4f () {};

	// other operators


	// methods
	Matrix4f& InitIdentity ();
	Matrix4f& operator*= (const Matrix4f& m);
};

inline Matrix4f operator*(Matrix4f m, Matrix4f& n)
{
	m *= n;
	return m;
}

#endif