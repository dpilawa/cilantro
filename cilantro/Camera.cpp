#include "Camera.h"

Camera::Camera (Vector3f& position, Vector3f& lookAt, Vector3f& up) :
cameraPosition (position), cameraLookAt (lookAt), cameraUp (up)
{
}

Camera::~Camera ()
{
}

Matrix4f Camera::GetViewMatrix () const
{
	return Mathf::GenCameraViewMatrix (cameraPosition, cameraLookAt, cameraUp);
}

Vector3f Camera::GetPosition () const
{
	return cameraPosition;
}
