#ifndef _MATRIX4F_H_
#define _MATRIX4F_H_

#include <algorithm> 

#define M(x, y) m [(x - 1) * 4 + y - 1]

class Matrix4f
{
	float m[16];
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

#endif