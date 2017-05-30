#include "Matrix4f.h"

void Matrix4f::SetXY (int x, int y, float v)
{
	M (x, y) = v;
}

float Matrix4f::getXY (int x, int y) const
{
	return M (x, y);
}

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

