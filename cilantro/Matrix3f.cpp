#include "Matrix3f.h"

Matrix3f& Matrix3f::InitIdentity ()
{
	(*this)[0][0] = 1.0f;
	(*this)[0][1] = 0.0f;
	(*this)[0][2] = 0.0f;

	(*this)[1][0] = 0.0f;
	(*this)[1][1] = 1.0f;
	(*this)[1][2] = 0.0f;

	(*this)[2][0] = 0.0f;
	(*this)[2][1] = 0.0f;
	(*this)[2][2] = 1.0f;

	return *this;
}



