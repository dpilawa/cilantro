#ifndef _MATRIX4F_H_
#define _MATRIX4F_H_

#include <algorithm> 

// row-major matrix representation in memory
#define M4(x, y) m [(x - 1) * 4 + y - 1]
#define MMUL4(m1, m2, x, y) M4(x, y) = m1.M4 (x, 1) * m2.M4 (1, y) + m1.M4 (x, 2) * m2.M4 (2, y) + m1.M4 (x, 3) * m2.M4 (3, y) + m1.M4 (x, 4) * m2.M4 (4, y)

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
	float GetXY (const int x, const int y) const;
	float* GetDataPointer ();

	// methods
	Matrix4f& InitIdentity ();

	// transpose matrix
	friend inline Matrix4f Transpose (const Matrix4f& m);

	// operators
	Matrix4f& operator*= (const Matrix4f& m);

private:

	float m[16];

};

// transpose matrix
inline Matrix4f Transpose (const Matrix4f & m)
{
	Matrix4f n;

	n.SetXY (1, 1, m.GetXY (1, 1));
	n.SetXY (1, 2, m.GetXY (2, 1));
	n.SetXY (1, 3, m.GetXY (3, 1));
	n.SetXY (1, 4, m.GetXY (4, 1));

	n.SetXY (2, 1, m.GetXY (1, 2));
	n.SetXY (2, 2, m.GetXY (2, 2));
	n.SetXY (2, 3, m.GetXY (3, 2));
	n.SetXY (2, 4, m.GetXY (4, 2));

	n.SetXY (3, 1, m.GetXY (1, 3));
	n.SetXY (3, 2, m.GetXY (2, 3));
	n.SetXY (3, 3, m.GetXY (3, 3));
	n.SetXY (3, 4, m.GetXY (4, 3));

	n.SetXY (4, 1, m.GetXY (1, 4));
	n.SetXY (4, 2, m.GetXY (2, 4));
	n.SetXY (4, 3, m.GetXY (3, 4));
	n.SetXY (4, 4, m.GetXY (4, 4));

	return n;
}

// compound assignment operator for matrix multiplication
inline Matrix4f& Matrix4f::operator*=(const Matrix4f& other)
{
	// actual multiplication code
	Matrix4f temp (*this);

	MMUL4 (temp, other, 1, 1);
	MMUL4 (temp, other, 1, 2);
	MMUL4 (temp, other, 1, 3);
	MMUL4 (temp, other, 1, 4);
	MMUL4 (temp, other, 2, 1);
	MMUL4 (temp, other, 2, 2);
	MMUL4 (temp, other, 2, 3);
	MMUL4 (temp, other, 2, 4);
	MMUL4 (temp, other, 3, 1);
	MMUL4 (temp, other, 3, 2);
	MMUL4 (temp, other, 3, 3);
	MMUL4 (temp, other, 3, 4);
	MMUL4 (temp, other, 4, 1);
	MMUL4 (temp, other, 4, 2);
	MMUL4 (temp, other, 4, 3);
	MMUL4 (temp, other, 4, 4);

	return *this;
}

// binary operator for matrix by matrix multiplication
inline Matrix4f operator* (Matrix4f m, const Matrix4f& n)
{
	m *= n;
	return m;
};

#endif