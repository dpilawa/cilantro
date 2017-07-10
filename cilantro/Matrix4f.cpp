#include "Matrix4f.h"

void Matrix4f::SetXY (int x, int y, float v)
{
	M4 (x, y) = v;
}

float Matrix4f::GetXY (int x, int y) const
{
	return M4 (x, y);
}

float * Matrix4f::getDataPointer ()
{
	return m;
}

Matrix4f& Matrix4f::InitIdentity()
{ 
	M4 (1, 1) = 1.0f;
	M4 (1, 2) = 0.0f;
	M4 (1, 3) = 0.0f;
	M4 (1, 4) = 0.0f;
	
	M4 (2, 1) = 0.0f;
	M4 (2, 2) = 1.0f;
	M4 (2, 3) = 0.0f;
	M4 (2, 4) = 0.0f;
	
	M4 (3, 1) = 0.0f;
	M4 (3, 2) = 0.0f;
	M4 (3, 3) = 1.0f;
	M4 (3, 4) = 0.0f;

	M4 (4, 1) = 0.0f;
	M4 (4, 2) = 0.0f;
	M4 (4, 3) = 0.0f;
	M4 (4, 4) = 1.0f;

	return *this;
}


