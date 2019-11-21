#include "cilantroengine.h"
#include "math/Matrix3f.h"
#include <algorithm>
#include <initializer_list>

#define M3(i, j) m[i * 3 + j]

// constructor
Matrix3f::Matrix3f () { };

// initializer list constructor
Matrix3f::Matrix3f (std::initializer_list<float> initializer)
{
    float* mPtr = m;

    for (auto&& i : initializer)
    {
        *mPtr++ = i;
    }
}

// copy constructor
Matrix3f::Matrix3f (const Matrix3f& other)
{
	std::copy (other.m, other.m + 9, m);
}

// copy constructor (submatrix)
Matrix3f::Matrix3f (const Matrix4f& other)
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
Matrix3f::Matrix3f (const Vector3f& c1, const Vector3f& c2, const Vector3f& c3)
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
Matrix3f::Matrix3f (Matrix3f&& other) : Matrix3f ()
{
	std::swap (m, other.m);
}

// assignment operator
Matrix3f& Matrix3f::operator= (Matrix3f other)
{
	std::swap (m, other.m);
	return *this;
}

// destructor
Matrix3f::~Matrix3f () {};

Matrix3f& Matrix3f::Matrix3f::InitIdentity ()
{
	M3 (0, 0) = 1.0f;
	M3 (0, 1) = 0.0f;
	M3 (0, 2) = 0.0f;

	M3 (1, 0) = 0.0f;
	M3 (1, 1) = 1.0f;
	M3 (1, 2) = 0.0f;

	M3 (2, 0) = 0.0f;
	M3 (2, 1) = 0.0f;
	M3 (2, 2) = 1.0f;

	return *this;
}

// mutator
float* Matrix3f::operator[](unsigned int index)
{
	return m + index * 3;
}

// accessor
const float* Matrix3f::operator[](unsigned int index) const
{
	return m + index * 3;
}

// compound assignment operator for matrix multiplication
Matrix3f& Matrix3f::operator*=(const Matrix3f& other)
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

// matrix by vector pre-multiplication 
Vector3f Matrix3f::operator*(const Vector3f& v) const
{
	Vector3f temp;

    temp[0] = M3 (0, 0) * v[0] + M3 (0, 1) * v[1] + M3 (0, 2) * v[2];
    temp[1] = M3 (1, 0) * v[0] + M3 (1, 1) * v[1] + M3 (1, 2) * v[2];
    temp[2] = M3 (2, 0) * v[0] + M3 (2, 1) * v[1] + M3 (2, 2) * v[2];

    return temp;
}

Matrix3f& Matrix3f::operator*=(float f)
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
Matrix3f operator* (Matrix3f m, const Matrix3f& n)
{
	m *= n;
	return m;
}

// binary operator for matrix by float multiplication
Matrix3f operator* (Matrix3f m, float f)
{
	m *= f;
	return m;
}

Matrix3f operator* (float f, Matrix3f m)
{
	m *= f;
	return m;
}

