#include "cilantroengine.h"
#include "math/Matrix3f.h"

Matrix3f& Matrix3f::InitIdentity ()
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



