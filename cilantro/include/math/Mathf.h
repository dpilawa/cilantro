#ifndef _MATHF_H_
#define _MATHF_H_

#include "cilantroengine.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include "math/Vector4f.h"
#include "math/Matrix3f.h"
#include "math/Matrix4f.h"
#include "math/Quaternion.h"
#include "math/GaussLegendreIntegrator.h"
#include <vector>

// Math constants
#define cPI 3.14159265358979323846f

// Collection of static helper math floating point routines
class Mathf
{
public:
    // static constants
    __EAPI static float Pi () { return cPI; }

    // float comparison
    __EAPI static bool VeryClose (const float a, const float b, int ulp);

    // vector operations
    __EAPI static float Length (const Vector3f& v);
    __EAPI static float Length (const Vector4f& v);
    __EAPI static Vector3f Normalize (const Vector3f& v);
    __EAPI static Vector4f Normalize (const Vector4f& v);
    __EAPI static float Dot (const Vector3f& v1, const Vector3f& v2);
    __EAPI static Vector3f Cross (const Vector3f& v1, const Vector3f& v2);

    // homogenous coordinates
    __EAPI static Vector4f CartesianToHomogenous (const Vector3f& v, float w);
    __EAPI static std::vector<Vector4f> CartesianToHomogenous (const std::vector<Vector3f>& v, const std::vector<float>& w);	
    __EAPI static Vector3f HomogenousToCartesianPerspective (const Vector4f& v);
    __EAPI static Vector3f HomogenousToCartesianTruncate (const Vector4f& v);
    __EAPI static float GetHomogenousWeight (const Vector4f& v);

    // quaterion operations
    __EAPI static float Norm (const Quaternion& q);
    __EAPI static float Dot (const Quaternion& q1, const Quaternion& q2);
    __EAPI static Quaternion Normalize (const Quaternion& q);
    __EAPI static Quaternion Conjugate (const Quaternion& q);
    __EAPI static Quaternion Invert (const Quaternion& q);	
    __EAPI static Quaternion Product (const Quaternion& q, const Quaternion& r);
    __EAPI static Quaternion GenRotationQuaternion (const Vector3f& axis, float theta);
    __EAPI static Vector3f Rotate (const Vector3f &v, const Quaternion& q);
    __EAPI static Vector3f Rotate (const Vector3f &v, const Vector3f& axis, float theta);
    __EAPI static Vector3f QuaternionToEuler (const Quaternion& q);
    __EAPI static Quaternion EulerToQuaterion (const Vector3f& euler);

    // interpolations
    __EAPI static float Clamp (float v0, float v1, float vt);
    __EAPI static float Step (float v0, float v1, float vt);
    __EAPI static float Smoothstep (float v0, float v1, float vt);
    __EAPI static float Smootherstep (float v0, float v1, float vt);

    __EAPI static float Lerp (float v0, float v1, float t);
    __EAPI static Vector3f Lerp (const Vector3f& v0, const Vector3f& v1, float t);
    __EAPI static Quaternion Lerp (const Quaternion& q0, const Quaternion& q1, float t);	
    __EAPI static Quaternion Slerp (const Quaternion& q0, const Quaternion& q1, float t);

    // numerical
    __EAPI static float Integral (float a, float b, std::function<float(float)> f);

    // matrix operations
    __EAPI static float Det (const Matrix3f& m);
    __EAPI static float Det (const Matrix4f& m);
    __EAPI static Matrix3f Transpose (const Matrix3f& m);
    __EAPI static Matrix4f Transpose (const Matrix4f& m);
    __EAPI static Matrix3f Invert (const Matrix3f& m);
    __EAPI static Matrix4f Invert (const Matrix4f& m);

    // combinatorics
    __EAPI static unsigned int Binomial (unsigned int n, unsigned int k);

    // convert between degrees and radians
    __EAPI static float Deg2Rad (float degrees);
    __EAPI static Vector3f Deg2Rad (const Vector3f &degrees);
    __EAPI static float Rad2Deg (float radians);
    __EAPI static Vector3f Rad2Deg (const Vector3f &radians);

    // convert spherical to cartesian coordinates (theta and pi in radians)
    __EAPI static Vector3f Spherical2Cartesian (float theta, float phi, float r);
    __EAPI static Vector2f Spherical2Cartesian (float theta, float r);

    // generate rotation matrix
    // angle provided in radians
    __EAPI static Matrix4f GenRotationXMatrix (float angle);
    __EAPI static Matrix4f GenRotationYMatrix (float angle);
    __EAPI static Matrix4f GenRotationZMatrix (float angle);

    // generate rotation matrix to rotate around X, then Y, then Z axis
    // angle provided in radians
    __EAPI static Matrix4f GenRotationXYZMatrix (float x, float y, float z);
    __EAPI static Matrix4f GenRotationXYZMatrix (const Vector3f& r);

    // convert between matrix amd quaternion
    __EAPI static Matrix4f GenRotationMatrix (const Quaternion& q);
    __EAPI static Quaternion GenQuaternion (const Matrix4f& m);

    // generate translation matrix along 3 axes
    __EAPI static Matrix4f GenTranslationMatrix (float x, float y, float z);
    __EAPI static Matrix4f GenTranslationMatrix (const Vector3f& t);

    // generate scaling matrix along 3 axes
    __EAPI static Matrix4f GenScalingMatrix (float x, float y, float z);
    __EAPI static Matrix4f GenScalingMatrix (const Vector3f& s);

    // generate camera (view) matrix or orientation quaternion
    __EAPI static Matrix4f GenCameraViewMatrix (const Vector3f& position, const Vector3f& lookAt, const Vector3f& up);
    __EAPI static Quaternion GenCameraOrientationQuaternion (const Vector3f& position, const Vector3f& lookAt, const Vector3f& up);

    // generate projection matrix (perspective)
    // fov provided in radians
    __EAPI static Matrix4f GenPerspectiveProjectionMatrix (float aspect, float fov, float nearZ, float farZ);

    // generate projection matrix (orthographic)
    __EAPI static Matrix4f GenOrthographicProjectionMatrix (float aspect, float width, float nearZ, float farZ);

    // system of linear equations
    template <typename T>
    __EAPI static void SolveSystemOfLinearEquations (std::vector<std::vector<float>>& A, std::vector<T>& b);

private:
    static GaussLegendreIntegrator<INTEGRATOR_DEGREE> integrator;
};

#endif

