#include <cmath>
#include "Mathf.h"


float Mathf::Deg2Rad (float degrees)
{
	return degrees * Mathf::Pi() / 180.f;
}


float Mathf::Rad2Deg (float radians)
{
	return radians * 180.f / Mathf::Pi();
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

Matrix4f Mathf::GenScalingMatrix (float x, float y, float z)
{
	Matrix4f m;

	m.InitIdentity ();
	m.SetXY (1, 1, x);
	m.SetXY (2, 2, y);
	m.SetXY (3, 3, z);

	return m;
}

