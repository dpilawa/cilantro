#include "OrthographicCamera.h"

OrthographicCamera::OrthographicCamera (Vector3f & position, Vector3f & lookAt, Vector3f & up, float width, float near, float far) :
	Camera (position, lookAt, up),
	cameraWidth (width), nearPlane (near), farPlane (far)
{
}

OrthographicCamera::~OrthographicCamera ()
{
}

Matrix4f OrthographicCamera::GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const
{
	return Mathf::GenOrthographicProjectionMatrix (float (xRes) / float (yRes), cameraWidth, nearPlane, farPlane);
}



