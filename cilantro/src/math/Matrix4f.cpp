#include "cilantroengine.h"
#include "math/Matrix4f.h"

Matrix4f& Matrix4f::InitIdentity()
{ 
	M4 (0, 0) = 1.0f;
	M4 (0, 1) = 0.0f;
	M4 (0, 2) = 0.0f;
	M4 (0, 3) = 0.0f;
	
	M4 (1, 0) = 0.0f;
	M4 (1, 1) = 1.0f;
	M4 (1, 2) = 0.0f;
	M4 (1, 3) = 0.0f;
	
	M4 (2, 0) = 0.0f;
	M4 (2, 1) = 0.0f;
	M4 (2, 2) = 1.0f;
	M4 (2, 3) = 0.0f;

	M4 (3, 0) = 0.0f;
	M4 (3, 1) = 0.0f;
	M4 (3, 2) = 0.0f;
	M4 (3, 3) = 1.0f;

	return *this;
}

