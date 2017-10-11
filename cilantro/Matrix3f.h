#ifndef _MATRIX3F_H_
#define _MATRIX3F_H_

#include "Matrix4f.h"

// row-major matrix representation in memory
#define M3(x, y) m [(x - 1) * 3 + y - 1]
#define MMUL3(m1, m2, x, y) m1.M3 (x, 1) * m2.M3 (1, y) + m1.M3 (x, 2) * m2.M3 (2, y) + m1.M3 (x, 3) * m2.M3 (3, y)

class Matrix3f
{
public:
	// constructor
	Matrix3f () { };

	// copy constructor
	Matrix3f (const Matrix3f& other)
	{
		std::copy (other.m, other.m + 16, m);
	}

	// copy constructor (submatrix)
	Matrix3f (const Matrix4f& other)
	{
		M3 (1, 1) = other[0][0];
		M3 (1, 2) = other[0][1];
		M3 (1, 3) = other[0][2];

		M3 (2, 1) = other[1][0];
		M3 (2, 2) = other[1][1];
		M3 (2, 3) = other[1][2];

		M3 (3, 1) = other[2][0];
		M3 (3, 2) = other[2][1];
		M3 (3, 3) = other[2][2];
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
	float GetDeterminant () const;

	// transpose matrix
	friend inline Matrix3f Transpose (const Matrix3f& m);

	// inverse matrix
	friend inline Matrix3f Inverse (const Matrix3f& m);

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

// transpose matrix
inline Matrix3f Transpose (const Matrix3f & m)
{
	Matrix3f n;

	n.M3 (1, 1) = m.M3 (1, 1);
	n.M3 (1, 2) = m.M3 (2, 1);
	n.M3 (1, 3) = m.M3 (3, 1);

	n.M3 (2, 1) = m.M3 (1, 2);
	n.M3 (2, 2) = m.M3 (2, 2);
	n.M3 (2, 3) = m.M3 (3, 2);

	n.M3 (3, 1) = m.M3 (1, 3);
	n.M3 (3, 2) = m.M3 (2, 3);
	n.M3 (3, 3) = m.M3 (3, 3);

	return n;
}

inline Matrix3f Inverse (const Matrix3f & m)
{
	Matrix3f i;
	float det = m.GetDeterminant();

	i.M3 (1, 1) = m.M3 (2, 2) * m.M3 (3, 3) - m.M3 (2, 3) * m.M3 (3, 2);
	i.M3 (1, 2) = m.M3 (1, 3) * m.M3 (3, 2) - m.M3 (1, 2) * m.M3 (3, 3);
	i.M3 (1, 3) = m.M3 (1, 2) * m.M3 (2, 3) - m.M3 (1, 3) * m.M3 (2, 2);

	i.M3 (2, 1) = m.M3 (2, 3) * m.M3 (3, 1) - m.M3 (2, 1) * m.M3 (3, 3);
	i.M3 (2, 2) = m.M3 (1, 1) * m.M3 (3, 3) - m.M3 (1, 3) * m.M3 (3, 1);
	i.M3 (2, 3) = m.M3 (1, 3) * m.M3 (2, 1) - m.M3 (1, 1) * m.M3 (2, 3);

	i.M3 (3, 1) = m.M3 (2, 1) * m.M3 (3, 2) - m.M3 (2, 2) * m.M3 (3, 1);
	i.M3 (3, 2) = m.M3 (1, 2) * m.M3 (3, 1) - m.M3 (1, 1) * m.M3 (3, 2);
	i.M3 (3, 3) = m.M3 (1, 1) * m.M3 (2, 2) - m.M3 (1, 2) * m.M3 (2, 1);

	i *= (1.0f / det);

	return i;
}

// compound assignment operator for matrix multiplication
inline Matrix3f& Matrix3f::operator*=(const Matrix3f& other)
{
	// actual multiplication code
	Matrix3f temp (*this);

	M3 (1, 1) = MMUL3 (temp, other, 1, 1);
	M3 (1, 2) = MMUL3 (temp, other, 1, 2);
	M3 (1, 3) = MMUL3 (temp, other, 1, 3);
	M3 (2, 1) = MMUL3 (temp, other, 2, 1);
	M3 (2, 2) = MMUL3 (temp, other, 2, 2);
	M3 (2, 3) = MMUL3 (temp, other, 2, 3);
	M3 (3, 1) = MMUL3 (temp, other, 3, 1);
	M3 (3, 2) = MMUL3 (temp, other, 3, 2);
	M3 (3, 3) = MMUL3 (temp, other, 3, 3);

	return *this;
}

inline Matrix3f & Matrix3f::operator*=(float f)
{
	M3 (1, 1) *= f;
	M3 (1, 2) *= f;
	M3 (1, 3) *= f;

	M3 (2, 1) *= f;
	M3 (2, 2) *= f;
	M3 (2, 3) *= f;

	M3 (3, 1) *= f;
	M3 (3, 2) *= f;
	M3 (3, 3) *= f;

	return *this;
}

// binary operator for matrix by matrix multiplication
inline Matrix3f operator* (Matrix3f m, const Matrix3f& n)
{
	m *= n;
	return m;
};

// binary operator for matrix by float multiplication
inline Matrix3f operator* (Matrix3f m, float f)
{
	m *= f;
	return m;
};

inline Matrix3f operator* (float f, Matrix3f m)
{
	m *= f;
	return m;
};

#endif