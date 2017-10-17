#include "PerspectiveCamera.h"

PerspectiveCamera::PerspectiveCamera (float fov, float near, float far) :
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



