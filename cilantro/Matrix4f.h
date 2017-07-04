#ifndef _MATRIX4F_H_
#define _MATRIX4F_H_

#include <algorithm> 

// row-major matrix representation in memory
#define M(x, y) m [(x - 1) * 4 + y - 1]
#define MMUL(m1, m2, x, y) M(x, y) = m1.M (x, 1) * m2.M (1, y) + m1.M (x, 2) * m2.M (2, y) + m1.M (x, 3) * m2.M (3, y) + m1.M (x, 4) * m2.M (4, y)

class Matrix4f
{
public:
	// constructor
	Matrix4f () { };
	
	// copy constructor
	Matrix4f (const Matrix4f& other)
	{
		std::copy (other.m, other.m + 16, m);
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
	float* getDataPointer ();

	// methods
	Matrix4f& InitIdentity ();

	// operators
	Matrix4f& operator*= (const Matrix4f& m);

private:

	float m[16];

};

// compound assignment operator for matrix multiplication
inline Matrix4f& Matrix4f::operator*=(const Matrix4f& other)
{
	// actual multiplication code
	Matrix4f temp (*this);

	MMUL (temp, other, 1, 1);
	MMUL (temp, other, 1, 2);
	MMUL (temp, other, 1, 3);
	MMUL (temp, other, 1, 4);
	MMUL (temp, other, 2, 1);
	MMUL (temp, other, 2, 2);
	MMUL (temp, other, 2, 3);
	MMUL (temp, other, 2, 4);
	MMUL (temp, other, 3, 1);
	MMUL (temp, other, 3, 2);
	MMUL (temp, other, 3, 3);
	MMUL (temp, other, 3, 4);
	MMUL (temp, other, 4, 1);
	MMUL (temp, other, 4, 2);
	MMUL (temp, other, 4, 3);
	MMUL (temp, other, 4, 4);

	return *this;
}

// binary operator for matrix multiplication
inline Matrix4f operator* (Matrix4f m, const Matrix4f& n)
{
	m *= n;
	return m;
};

#endif