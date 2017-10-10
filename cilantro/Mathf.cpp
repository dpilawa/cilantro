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
	m.SetXY (2, 2, std::cos (angle));
	m.SetXY (2, 3, -std::sin (angle));
	m.SetXY (3, 2, std::sin (angle));
	m.SetXY (3, 3, std::cos (angle));
	
	return m;
}


Matrix4f Mathf::GenRotationYMatrix (float angle)
{
	Matrix4f m;

	m.InitIdentity ();
	m.SetXY (1, 1, std::cos (angle));
	m.SetXY (1, 3, -std::sin (angle));
	m.SetXY (3, 1, std::sin (angle));
	m.SetXY (3, 3, std::cos (angle));

	return m;
}


Matrix4f Mathf::GenRotationZMatrix (float angle)
{
	Matrix4f m;

	m.InitIdentity ();
	m.SetXY (1, 1, std::cos (angle));
	m.SetXY (1, 2, -std::sin (angle));
	m.SetXY (2, 1, std::sin (angle));
	m.SetXY (2, 2, std::cos (angle));

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
	m.SetXY (1, 4, x);
	m.SetXY (2, 4, y);
	m.SetXY (3, 4, z);

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
	m.SetXY (1, 1, x);
	m.SetXY (2, 2, y);
	m.SetXY (3, 3, z);

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
	m.SetXY (1, 1, u[0]);
	m.SetXY (1, 2, u[1]);
	m.SetXY (1, 3, u[2]);
	m.SetXY (2, 1, v[0]);
	m.SetXY (2, 2, v[1]);
	m.SetXY (2, 3, v[2]);
	m.SetXY (3, 1, n[0]);
	m.SetXY (3, 2, n[1]);
	m.SetXY (3, 3, n[2]);

	return m * Mathf::GenTranslationMatrix (-position);
}

Matrix4f Mathf::GenPerspectiveProjectionMatrix (float aspect, float fov, float nearZ, float farZ)
{
	Matrix4f m;
	float scale = std::tanf (fov * 0.5f);

	m.InitIdentity ();
	m.SetXY (1, 1, 1.0f / (aspect * scale));
	m.SetXY (2, 2, 1.0f / scale);
	m.SetXY (3, 3, (-nearZ - farZ) / (nearZ - farZ));
	m.SetXY (3, 4, (2.0f * nearZ * farZ) / (nearZ - farZ));
	m.SetXY (4, 3, 1.0f);

	return m;
}

Matrix4f Mathf::GenOrthographicProjectionMatrix (float aspect, float width, float nearZ, float farZ)
{
	Matrix4f m;

	m.InitIdentity ();
	m.SetXY (1, 1, 2.0f / width);
	m.SetXY (2, 2, (2.0f * aspect) / width);
	m.SetXY (3, 3, 2.0f / (farZ - nearZ));
	m.SetXY (3, 4, (-farZ - nearZ) / (farZ - nearZ));

	return m;
}

