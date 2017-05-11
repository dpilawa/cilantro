#ifndef _MATHF_H_
#define _MATHF_H_

#include "Matrix4f.h"

// Math constants
#define M_PI 3.14159265358979323846f

// Collection of static helper math floating point routines
class Mathf
{
public:
	// static constants
	static const float Pi () { return M_PI; };

	// degres & radian conversion
	static float Deg2Rad (float degrees);
	static float Rad2Deg (float radians);

	// generation of transformation matrices
	static Matrix4f GenRotationXMatrix (float angle);
	static Matrix4f GenRotationYMatrix (float angle);
	static Matrix4f GenRotationZMatrix (float angle);
	static Matrix4f GenTranslationMatrix (float x, float y, float z);
	static Matrix4f GenScalingMatrix (float x, float y, float z);
};

#endif

