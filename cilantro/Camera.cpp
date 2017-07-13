#include "Camera.h"

Camera::Camera (Vector3f & position, Vector3f & lookAt, Vector3f & up, float fov, float near, float far) :
	cameraPosition (position), cameraLookAt (lookAt), cameraUp (up),
	cameraFOV (fov), nearPlane (near), farPlane (far)
{
}

Camera::~Camera ()
{
}

Matrix4f Camera::GetViewMatrix () const
{
	return Mathf::GenCameraViewMatrix (cameraPosition, cameraLookAt, cameraUp);
}

Matrix4f Camera::GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const
{
	return Mathf::GenPerspectiveProjectionMatrix (float (xRes) / float (yRes), Mathf::Deg2Rad (cameraFOV), nearPlane, farPlane);
}


