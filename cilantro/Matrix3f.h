#ifndef _MATRIX3F_H_
#define _MATRIX3F_H_

#include <algorithm>
#include "Matrix4f.h"

// row-major matrix representation in memory
#define M3(x, y) m [(x - 1) * 3 + y - 1]
#define MMUL3(m1, m2, x, y) M3(x, y) = m1.M3 (x, 1) * m2.M3 (1, y) + m1.M3 (x, 2) * m2.M3 (2, y) + m1.M3 (x, 3) * m2.M3 (3, y)

class Matrix3f
{
public:
	// constructor
	Matrix3f () {};

	// copy constructor
	Matrix3f (const Matrix3f& other)
	{
		std::copy (other.m, other.m + 16, m);
	}

	// copy constructor (submatrix)
	Matrix3f (const Matrix4f& other)
	{
		M3 (1, 1) = other.GetXY (1, 1);
		M3 (1, 2) = other.GetXY (1, 2);
		M3 (1, 3) = other.GetXY (1, 3);

		M3 (2, 1) = other.GetXY (2, 1);
		M3 (2, 2) = other.GetXY (2, 2);
		M3 (2, 3) = other.GetXY (2, 3);

		M3 (3, 1) = other.GetXY (3, 1);
		M3 (3, 2) = other.GetXY (3, 2);
		M3 (3, 3) = other.GetXY (3, 3);
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

	// getters & setters
	void SetXY (const int x, const int y, const float v);
	float GetXY (const int x, const int y) const;
	float* getDataPointer ();

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

// transpose matrix
inline Matrix3f Transpose (const Matrix3f & m)
{
	Matrix3f n;

	n.SetXY (1, 1, m.GetXY (1, 1));
	n.SetXY (1, 2, m.GetXY (2, 1));
	n.SetXY (1, 3, m.GetXY (3, 1));

	n.SetXY (2, 1, m.GetXY (1, 2));
	n.SetXY (2, 2, m.GetXY (2, 2));
	n.SetXY (2, 3, m.GetXY (3, 2));

	n.SetXY (3, 1, m.GetXY (1, 3));
	n.SetXY (3, 2, m.GetXY (2, 3));
	n.SetXY (3, 3, m.GetXY (3, 3));

	return n;
}

inline Matrix3f Inverse (const Matrix3f & m)
{
	Matrix3f i;
	float det = m.GetDeterminant();

	i.SetXY (1, 1, m.GetXY (2, 2) * m.GetXY (3, 3) - m.GetXY (2, 3) * m.GetXY (3, 2));
	i.SetXY (1, 2, m.GetXY (1, 3) * m.GetXY (3, 2) - m.GetXY (1, 2) * m.GetXY (3, 3));
	i.SetXY (1, 3, m.GetXY (1, 2) * m.GetXY (2, 3) - m.GetXY (1, 3) * m.GetXY (2, 2));

	i.SetXY (2, 1, m.GetXY (2, 3) * m.GetXY (3, 1) - m.GetXY (2, 1) * m.GetXY (3, 3));
	i.SetXY (2, 2, m.GetXY (1, 1) * m.GetXY (3, 3) - m.GetXY (1, 3) * m.GetXY (3, 1));
	i.SetXY (2, 3, m.GetXY (1, 3) * m.GetXY (2, 1) - m.GetXY (1, 1) * m.GetXY (2, 3));

	i.SetXY (3, 1, m.GetXY (2, 1) * m.GetXY (3, 2) - m.GetXY (2, 2) * m.GetXY (3, 1));
	i.SetXY (3, 2, m.GetXY (1, 2) * m.GetXY (3, 1) - m.GetXY (1, 1) * m.GetXY (3, 2));
	i.SetXY (3, 3, m.GetXY (1, 1) * m.GetXY (2, 2) - m.GetXY (1, 2) * m.GetXY (2, 1));

	i *= (1.0f / det);

	return i;
}

// compound assignment operator for matrix multiplication
inline Matrix3f& Matrix3f::operator*=(const Matrix3f& other)
{
	// actual multiplication code
	Matrix3f temp (*this);

	MMUL3 (temp, other, 1, 1);
	MMUL3 (temp, other, 1, 2);
	MMUL3 (temp, other, 1, 3);
	MMUL3 (temp, other, 2, 1);
	MMUL3 (temp, other, 2, 2);
	MMUL3 (temp, other, 2, 3);
	MMUL3 (temp, other, 3, 1);
	MMUL3 (temp, other, 3, 2);
	MMUL3 (temp, other, 3, 3);

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