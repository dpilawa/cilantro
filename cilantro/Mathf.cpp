#include "Mathf.h"


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
	return Mathf::GenTranslationMatrix (t.GetX (), t.GetY (), t.GetZ ());
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
	m.SetXY (1, 1, u.GetX ());
	m.SetXY (1, 2, u.GetY ());
	m.SetXY (1, 3, u.GetZ ());
	m.SetXY (2, 1, v.GetX ());
	m.SetXY (2, 2, v.GetY ());
	m.SetXY (2, 3, v.GetZ ());
	m.SetXY (3, 1, n.GetX ());
	m.SetXY (3, 2, n.GetY ());
	m.SetXY (3, 3, n.GetZ ());

	return m * Mathf::GenTranslationMatrix (-position);
}

Matrix4f Mathf::GenPerspectiveProjectionMatrix (float aspect, float fov, float nearZ, float farZ)
{
	Matrix4f m;
	float fov2 = fov * 0.5f;

	m.InitIdentity ();
	m.SetXY (1, 1, 1.0f / (aspect * std::tanf (fov2)));
	m.SetXY (2, 2, 1.0f / std::tanf (fov2));
	m.SetXY (3, 3, (-nearZ - farZ) / (nearZ - farZ));
	m.SetXY (3, 4, (2.0f * nearZ * farZ) / (nearZ - farZ));
	m.SetXY (4, 3, 1.0f);

	return m;
}

