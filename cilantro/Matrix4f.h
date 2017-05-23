#ifndef _MATRIX4F_H_
#define _MATRIX4F_H_

#include <algorithm> 

// row-major matrix representation in memory
#define M(x, y) m [(x - 1) * 4 + y - 1]
#define MMUL(m1, m2, x, y) M(x, y) = m1.M (x, 1) * m2.M (1, y) + m1.M (x, 2) * m2.M (2, y) + m1.M (x, 3) * m2.M (3, y) + m1.M (x, 4) * m2.M (4, y)

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

	// getters & setters
	void SetXY (const int x, const int y, const float v);
	float getXY (const int x, const int y) const;

	// methods
	Matrix4f& InitIdentity ();
	Matrix4f& operator*= (const Matrix4f& m);
};

// binary operator for matrix multiplication
inline Matrix4f operator*(Matrix4f m, Matrix4f& n)
{
	m *= n;
	return m;
};

#endif