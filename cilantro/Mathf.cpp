#include <cmath>
#include "Mathf.h"

// convert degrees to radians
float Mathf::Deg2Rad (float degrees)
{
	return degrees * Mathf::Pi() / 180.f;
}

// convert radians to degrees
float Mathf::Rad2Deg (float radians)
{
	return radians * 180.f / Mathf::Pi();
}

// generate rotation matrix to rotate around X axis
// angle provided in radians
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

// generate rotation matrix to rotate around Y axis
// angle provided in radians
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

// generate rotation matrix to rotate around Z axis
// angle provided in radians
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

// generate translation matrix along 3 axes
Matrix4f GenTranslationMatrix (float x, float y, float z)
{
	Matrix4f m;

	m.InitIdentity ();
	m.SetXY (1, 4, x);
	m.SetXY (2, 4, y);
	m.SetXY (3, 4, z);

	return m;
}

// generate scaling matrix along 3 axes
Matrix4f GenScalingMatrix (float x, float y, float z)
{
	Matrix4f m;

	m.InitIdentity ();
	m.SetXY (1, 1, x);
	m.SetXY (2, 2, y);
	m.SetXY (3, 3, z);

	return m;
}
