#include "Mathf.h"


float Mathf::Length (const Vector3f & v)
{
	return std::sqrtf (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

Vector3f Mathf::Cross (const Vector3f & v1, const Vector3f & v2)
{
	return Vector3f (v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]);
}

float Mathf::Deg2Rad (float degrees)
{
	return degrees * Mathf::Pi() / 180.f;
}


float Mathf::Rad2Deg (float radians)
{
	return radians * 180.f / Mathf::Pi();
}

Vector3f Mathf::Spherical2Cartesian (float theta, float phi, float r)
{
	return Vector3f (r * std::sin (theta) * std::cos (phi), r * std::cos (theta), r * std::sin (theta) * std::sin (phi));
}


Matrix4f Mathf::GenRotationXMatrix(float angle)
{
	Matrix4f m;

	m.InitIdentity ();
	m[1][1] = std::cos (angle);
	m[1][2] = -std::sin (angle);
	m[2][1] = std::sin (angle);
	m[2][2] = std::cos (angle);
	
	return m;
}


Matrix4f Mathf::GenRotationYMatrix (float angle)
{
	Matrix4f m;

	m.InitIdentity ();
	m[0][0] = std::cos (angle);
	m[0][2] = -std::sin (angle);
	m[2][0] = std::sin (angle);
	m[2][2] = std::cos (angle);

	return m;
}


Matrix4f Mathf::GenRotationZMatrix (float angle)
{
	Matrix4f m;

	m.InitIdentity ();
	m[0][0] = std::cos (angle);
	m[0][1] = -std::sin (angle);
	m[1][0] = std::sin (angle);
	m[1][1] = std::cos (angle);

	return m;
}

Matrix4f Mathf::GenRotationXYZMatrix (float x, float y, float z)
{
	return GenRotationZMatrix (z) * GenRotationYMatrix (y) * GenRotationXMatrix (x);
}

Matrix4f Mathf::GenTranslationMatrix (float x, float y, float z)
{
	Matrix4f m;

	m.InitIdentity ();
	m[0][3] = x;
	m[1][3] = y;
	m[2][3] = z;

	return m;
}

Matrix4f Mathf::GenTranslationMatrix (Vector3f & t)
{
	return Mathf::GenTranslationMatrix (t[0], t[1], t[2]);
}

Matrix4f Mathf::GenScalingMatrix (float x, float y, float z)
{
	Matrix4f m;

	m.InitIdentity ();
	m[0][0] = x;
	m[1][1] = y;
	m[2][2] = z;

	return m;
}

Matrix4f Mathf::GenCameraViewMatrix (const Vector3f& position, const Vector3f& lookAt, const Vector3f& up)
{
	Matrix4f m;
	Vector3f u, v, n;

	n = lookAt - position;
	n.Normalize ();

	u = Cross (n, up);
	u.Normalize ();

	v = Cross (u, n);
	
	m.InitIdentity ();
	m[0][0] = u[0];
	m[0][1] = u[1];
	m[0][2] = u[2];
	m[1][0] = v[0];
	m[1][1] = v[1];
	m[1][2] = v[2];
	m[2][0] = n[0];
	m[2][1] = n[1];
	m[2][2] = n[2];

	return m * Mathf::GenTranslationMatrix (-position);
}

Matrix4f Mathf::GenPerspectiveProjectionMatrix (float aspect, float fov, float nearZ, float farZ)
{
	Matrix4f m;
	float scale = std::tanf (fov * 0.5f);

	m.InitIdentity ();
	m[0][0] = 1.0f / (aspect * scale);
	m[1][1] = 1.0f / scale;
	m[2][2] = (-nearZ - farZ) / (nearZ - farZ);
	m[2][3] = (2.0f * nearZ * farZ) / (nearZ - farZ);
	m[3][2] = 1.0f;

	return m;
}

Matrix4f Mathf::GenOrthographicProjectionMatrix (float aspect, float width, float nearZ, float farZ)
{
	Matrix4f m;

	m.InitIdentity ();
	m[0][0] = 2.0f / width;
	m[1][1] = (2.0f * aspect) / width;
	m[2][2] = 2.0f / (farZ - nearZ);
	m[2][3] = (-farZ - nearZ) / (farZ - nearZ);

	return m;
}

