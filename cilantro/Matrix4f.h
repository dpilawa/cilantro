#ifndef _MATRIX4F_H_
#define _MATRIX4F_H_

#include <algorithm>

#define M4(i, j) m[i * 4 + j]

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

	// accessor and mutator
	float* operator[](unsigned int index);
	const float* operator[](unsigned int index) const;

	// methods
	Matrix4f& InitIdentity ();

	// operators
	Matrix4f& operator*= (const Matrix4f& m);

private:

	float m[16];

};

// mutator
inline float* Matrix4f::operator[](unsigned int index)
{
	return m + index * 4;
}

// accessor
inline const float* Matrix4f::operator[](unsigned int index) const
{
	return m + index * 4;
}

// compound assignment operator for matrix multiplication
inline Matrix4f& Matrix4f::operator*=(const Matrix4f& other)
{
	// actual multiplication code
	Matrix4f temp (*this);

	M4 (0, 0) = temp[0][0] * other[0][0] + temp[0][1] * other[1][0] + temp[0][2] * other[2][0] + temp[0][3] * other[3][0];
	M4 (0, 1) = temp[0][0] * other[0][1] + temp[0][1] * other[1][1] + temp[0][2] * other[2][1] + temp[0][3] * other[3][1];
	M4 (0, 2) = temp[0][0] * other[0][2] + temp[0][1] * other[1][2] + temp[0][2] * other[2][2] + temp[0][3] * other[3][2];
	M4 (0, 3) = temp[0][0] * other[0][3] + temp[0][1] * other[1][3] + temp[0][2] * other[2][3] + temp[0][3] * other[3][3];

	M4 (1, 0) = temp[1][0] * other[0][0] + temp[1][1] * other[1][0] + temp[1][2] * other[2][0] + temp[1][3] * other[3][0];
	M4 (1, 1) = temp[1][0] * other[0][1] + temp[1][1] * other[1][1] + temp[1][2] * other[2][1] + temp[1][3] * other[3][1];
	M4 (1, 2) = temp[1][0] * other[0][2] + temp[1][1] * other[1][2] + temp[1][2] * other[2][2] + temp[1][3] * other[3][2];
	M4 (1, 3) = temp[1][0] * other[0][3] + temp[1][1] * other[1][3] + temp[1][2] * other[2][3] + temp[1][3] * other[3][3];

	M4 (2, 0) = temp[2][0] * other[0][0] + temp[2][1] * other[1][0] + temp[2][2] * other[2][0] + temp[2][3] * other[3][0];
	M4 (2, 1) = temp[2][0] * other[0][1] + temp[2][1] * other[1][1] + temp[2][2] * other[2][1] + temp[2][3] * other[3][1];
	M4 (2, 2) = temp[2][0] * other[0][2] + temp[2][1] * other[1][2] + temp[2][2] * other[2][2] + temp[2][3] * other[3][2];
	M4 (2, 3) = temp[2][0] * other[0][3] + temp[2][1] * other[1][3] + temp[2][2] * other[2][3] + temp[2][3] * other[3][3];

	M4 (3, 0) = temp[3][0] * other[0][0] + temp[3][1] * other[1][0] + temp[3][2] * other[2][0] + temp[3][3] * other[3][0];
	M4 (3, 1) = temp[3][0] * other[0][1] + temp[3][1] * other[1][1] + temp[3][2] * other[2][1] + temp[3][3] * other[3][1];
	M4 (3, 2) = temp[3][0] * other[0][2] + temp[3][1] * other[1][2] + temp[3][2] * other[2][2] + temp[3][3] * other[3][2];
	M4 (3, 3) = temp[3][0] * other[0][3] + temp[3][1] * other[1][3] + temp[3][2] * other[2][3] + temp[3][3] * other[3][3];

	return *this;
}

// binary operator for matrix by matrix multiplication
inline Matrix4f operator* (Matrix4f m, const Matrix4f& n)
{
	m *= n;
	return m;
};

#endif