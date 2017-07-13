#ifndef _MATHF_H_
#define _MATHF_H_

#include "Matrix4f.h"
#include "Vector3f.h"

// Math constants
#define cPI 3.14159265358979323846f

// Collection of static helper math floating point routines
class Mathf
{
public:
	// static constants
	static const float Pi () { return cPI; };

	// convert degrees to radians
	static float Deg2Rad (float degrees);
	// convert radians to degrees
	static float Rad2Deg (float radians);

	// generate rotation matrix to rotate around X axis
	// angle provided in radians
	static Matrix4f GenRotationXMatrix (float angle);

	// generate rotation matrix to rotate around Y axis
	// angle provided in radians
	static Matrix4f GenRotationYMatrix (float angle);

	// generate rotation matrix to rotate around Z axis
	// angle provided in radians
	static Matrix4f GenRotationZMatrix (float angle);

	// generate rotation matrix to rotate around X, then Y, then Z axis
	// angle provided in radians
	static Matrix4f GenRotationXYZMatrix (float x, float y, float z);

	// generate translation matrix along 3 axes
	static Matrix4f GenTranslationMatrix (float x, float y, float z);
	static Matrix4f GenTranslationMatrix (Vector3f& t);

	// generate scaling matrix along 3 axes
	static Matrix4f GenScalingMatrix (float x, float y, float z);

	// generate projection matrix (perspective)
	// fov provided in radians
	static Matrix4f GenCameraViewMatrix (const Vector3f& position, const Vector3f& lookAt, const Vector3f& up);

	// generate projection matrix (perspective)
	// fov provided in radians
	static Matrix4f GenPerspectiveProjectionMatrix (float aspect, float fov, float nearZ, float farZ);
};

#endif

