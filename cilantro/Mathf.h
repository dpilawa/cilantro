#ifndef _MATHF_H_
#define _MATHF_H_

#include "cilantroengine.h"
#include "Vector3f.h"
#include "Vector4f.h"
#include "Matrix3f.h"
#include "Matrix4f.h"

// Math constants
#define cPI 3.14159265358979323846f

// Collection of static helper math floating point routines
class Mathf
{
public:
	// static constants
	__EAPI static const float Pi () { return cPI; };

	// vector operations

	// returns vector length
	__EAPI static float Length (const Vector3f& v);
	// normalize vector
	__EAPI static Vector3f Normalize (const Vector3f& v);
	// cross product of two vectors
	__EAPI static Vector3f Cross (const Vector3f& v1, const Vector3f& v2);

	// matrix operations
	__EAPI static float Det (const Matrix3f& m);
	__EAPI static float Det (const Matrix4f& m);
	__EAPI static Matrix3f Transpose (const Matrix3f& m);
	__EAPI static Matrix4f Transpose (const Matrix4f& m);
	__EAPI static Matrix3f Invert (const Matrix3f& m);
	__EAPI static Matrix4f Invert (const Matrix4f& m);

	// convert degrees to radians
	__EAPI static float Deg2Rad (float degrees);
	// convert radians to degrees
	__EAPI static float Rad2Deg (float radians);

	// convert spherical to cartesian coordinates (theta and pi in radians)
	__EAPI static Vector3f Spherical2Cartesian (float theta, float phi, float r);

	// generate rotation matrix to rotate around X axis
	// angle provided in radians
	__EAPI static Matrix4f GenRotationXMatrix (float angle);

	// generate rotation matrix to rotate around Y axis
	// angle provided in radians
	__EAPI static Matrix4f GenRotationYMatrix (float angle);

	// generate rotation matrix to rotate around Z axis
	// angle provided in radians
	__EAPI static Matrix4f GenRotationZMatrix (float angle);

	// generate rotation matrix to rotate around X, then Y, then Z axis
	// angle provided in radians
	__EAPI static Matrix4f GenRotationXYZMatrix (float x, float y, float z);
	__EAPI static Matrix4f GenRotationXYZMatrix (Vector3f& r);

	// generate translation matrix along 3 axes
	__EAPI static Matrix4f GenTranslationMatrix (float x, float y, float z);
	__EAPI static Matrix4f GenTranslationMatrix (Vector3f& t);

	// generate scaling matrix along 3 axes
	__EAPI static Matrix4f GenScalingMatrix (float x, float y, float z);
	__EAPI static Matrix4f GenScalingMatrix (Vector3f& s);

	// generate camera (view) matrix
	__EAPI static Matrix4f GenCameraViewMatrix (const Vector3f& position, const Vector3f& lookAt, const Vector3f& up);

	// generate projection matrix (perspective)
	// fov provided in radians
	__EAPI static Matrix4f GenPerspectiveProjectionMatrix (float aspect, float fov, float nearZ, float farZ);

	// generate projection matrix (orthographic)
	__EAPI static Matrix4f GenOrthographicProjectionMatrix (float aspect, float width, float nearZ, float farZ);
};

#endif

