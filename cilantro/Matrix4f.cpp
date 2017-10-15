#include "Matrix4f.h"

Matrix4f& Matrix4f::InitIdentity()
{ 
	(*this)[0][0] = 1.0f;
	(*this)[0][1] = 0.0f;
	(*this)[0][2] = 0.0f;
	(*this)[0][3] = 0.0f;
	
	(*this)[1][0] = 0.0f;
	(*this)[1][1] = 1.0f;
	(*this)[1][2] = 0.0f;
	(*this)[1][3] = 0.0f;
	
	(*this)[2][0] = 0.0f;
	(*this)[2][1] = 0.0f;
	(*this)[2][2] = 1.0f;
	(*this)[2][3] = 0.0f;

	(*this)[3][0] = 0.0f;
	(*this)[3][1] = 0.0f;
	(*this)[3][2] = 0.0f;
	(*this)[3][3] = 1.0f;

	return *this;
}


