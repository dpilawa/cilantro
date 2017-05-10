#include "Matrix4f.h"

Matrix4f& Matrix4f::InitIdentity()
{ 
	M (1, 1) = 1.0f;
	M (1, 2) = 0.0f;
	M (1, 3) = 0.0f;
	M (1, 4) = 0.0f;
	
	M (2, 1) = 0.0f;
	M (2, 2) = 1.0f;
	M (2, 3) = 0.0f;
	M (2, 4) = 0.0f;
	
	M (3, 1) = 0.0f;
	M (3, 2) = 0.0f;
	M (3, 3) = 1.0f;
	M (3, 4) = 0.0f;

	M (4, 1) = 0.0f;
	M (4, 2) = 0.0f;
	M (4, 3) = 0.0f;
	M (4, 4) = 1.0f;

	return *this;
}

// binary operator for matrix multiplication
inline Matrix4f operator*(Matrix4f m, Matrix4f& n)
{
	m *= n;
	return m;
}

// compound assignment operator for matrix multiplication
Matrix4f& Matrix4f::operator*=(const Matrix4f& other)
{
	// actual multiplication code
	Matrix4f temp = *this;

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
