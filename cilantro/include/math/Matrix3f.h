#ifndef _MATRIX3F_H_
#define _MATRIX3F_H_

#include "math/Matrix4f.h"
#include <algorithm>

#define M3(i, j) m[i * 3 + j]

class Matrix3f
{
public:
	// constructor
	Matrix3f () { };

	// copy constructor
	Matrix3f (const Matrix3f& other)
	{
		std::copy (other.m, other.m + 9, m);
	}

	// copy constructor (submatrix)
	Matrix3f (const Matrix4f& other)
	{
		M3 (0, 0) = other[0][0];
		M3 (0, 1) = other[0][1];
		M3 (0, 2) = other[0][2];

		M3 (1, 0) = other[1][0];
		M3 (1, 1) = other[1][1];
		M3 (1, 2) = other[1][2];

		M3 (2, 0) = other[2][0];
		M3 (2, 1) = other[2][1];
		M3 (2, 2) = other[2][2];
	}

	// copy constructor (column vectors)
	Matrix3f (const Vector3f& c1, const Vector3f& c2, const Vector3f& c3)
	{
		M3 (0, 0) = c1[0];
		M3 (1, 0) = c1[1];
		M3 (2, 0) = c1[2];

		M3 (0, 1) = c2[0];
		M3 (1, 1) = c2[1];
		M3 (2, 1) = c2[2];

		M3 (0, 2) = c3[0];
		M3 (1, 2) = c3[1];
		M3 (2, 2) = c3[2];
	}

	// move constructor
	Matrix3f (Matrix3f&& other) : Matrix3f ()
	{
		std::swap (m, other.m);
	}

	// assignment operator
	Matrix3f& operator= (Matrix3f other)
	{
		std::swap (m, other.m);
		return *this;
	}

	// destructor
	~Matrix3f () {};

	// accessor and mutator
	float* operator[](unsigned int index);
	const float* operator[](unsigned int index) const;

	// methods
	Matrix3f& InitIdentity ();

	// operators
	Matrix3f& operator*= (const Matrix3f& m);
	Matrix3f& operator*= (float f);

private:

	float m[9];

};

// mutator
inline float* Matrix3f::operator[](unsigned int index)
{
	return m + index * 3;
}

// accessor
inline const float* Matrix3f::operator[](unsigned int index) const
{
	return m + index * 3;
}

// compound assignment operator for matrix multiplication
inline Matrix3f& Matrix3f::operator*=(const Matrix3f& other)
{
	// actual multiplication code
	Matrix3f temp (*this);

	M3 (0, 0) = temp[0][0] * other[0][0] + temp[0][1] * other[1][0] + temp[0][2] * other[2][0];
	M3 (0, 1) = temp[0][0] * other[0][1] + temp[0][1] * other[1][1] + temp[0][2] * other[2][1];
	M3 (0, 2) = temp[0][0] * other[0][2] + temp[0][1] * other[1][2] + temp[0][2] * other[2][2];

	M3 (1, 0) = temp[1][0] * other[0][0] + temp[1][1] * other[1][0] + temp[1][2] * other[2][0];
	M3 (1, 1) = temp[1][0] * other[0][1] + temp[1][1] * other[1][1] + temp[1][2] * other[2][1];
	M3 (1, 2) = temp[1][0] * other[0][2] + temp[1][1] * other[1][2] + temp[1][2] * other[2][2];

	M3 (2, 0) = temp[2][0] * other[0][0] + temp[2][1] * other[1][0] + temp[2][2] * other[2][0];
	M3 (2, 1) = temp[2][0] * other[0][1] + temp[2][1] * other[1][1] + temp[2][2] * other[2][1];
	M3 (2, 2) = temp[2][0] * other[0][2] + temp[2][1] * other[1][2] + temp[2][2] * other[2][2];

	return *this;
}

inline Matrix3f & Matrix3f::operator*=(float f)
{
	M3 (0, 0) *= f;
	M3 (0, 1) *= f;
	M3 (0, 2) *= f;

	M3 (1, 0) *= f;
	M3 (1, 1) *= f;
	M3 (1, 2) *= f;

	M3 (2, 0) *= f;
	M3 (2, 1) *= f;
	M3 (2, 2) *= f;

	return *this;
}

// binary operator for matrix by matrix multiplication
inline Matrix3f operator* (Matrix3f m, const Matrix3f& n)
{
	m *= n;
	return m;
}

// binary operator for matrix by float multiplication
inline Matrix3f operator* (Matrix3f m, float f)
{
	m *= f;
	return m;
}

inline Matrix3f operator* (float f, Matrix3f m)
{
	m *= f;
	return m;
}

#endif