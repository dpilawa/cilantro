#include "PerspectiveCamera.h"

PerspectiveCamera::PerspectiveCamera (Vector3f & position, Vector3f & lookAt, Vector3f & up, float fov, float near, float far) :
	Camera (position, lookAt, up),
	cameraFOV (fov), nearPlane (near), farPlane (far)
{
}

PerspectiveCamera::~PerspectiveCamera ()
{
}

Matrix4f PerspectiveCamera::GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const
{
	return Mathf::GenPerspectiveProjectionMatrix (float (xRes) / float (yRes), Mathf::Deg2Rad (cameraFOV), nearPlane, farPlane);
}



