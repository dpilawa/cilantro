#include "Matrix3f.h"

void Matrix3f::SetXY (int x, int y, float v)
{
	M3 (x, y) = v;
}

float Matrix3f::GetXY (int x, int y) const
{
	return M3 (x, y);
}

float * Matrix3f::GetDataPointer ()
{
	return m;
}

Matrix3f& Matrix3f::InitIdentity ()
{
	M3 (1, 1) = 1.0f;
	M3 (1, 2) = 0.0f;
	M3 (1, 3) = 0.0f;

	M3 (2, 1) = 0.0f;
	M3 (2, 2) = 1.0f;
	M3 (2, 3) = 0.0f;

	M3 (3, 1) = 0.0f;
	M3 (3, 2) = 0.0f;
	M3 (3, 3) = 1.0f;

	return *this;
}

float Matrix3f::GetDeterminant () const
{
	return M3 (1, 1) * M3 (2, 2) * M3 (3, 3)
		 - M3 (1, 2) * M3 (2, 1) * M3 (3, 3)
		 - M3 (1, 1) * M3 (2, 3) * M3 (3, 2)
		 + M3 (1, 3) * M3 (2, 1) * M3 (3, 2)
		 + M3 (1, 2) * M3 (2, 3) * M3 (3, 1)
		 - M3 (1, 3) * M3 (2, 2) * M3 (3, 1);
}


