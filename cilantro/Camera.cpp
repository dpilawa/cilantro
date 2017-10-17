#include "Camera.h"

Camera::Camera ()
{
}

Camera::~Camera ()
{
}

Matrix4f Camera::GetViewMatrix ()
{
	Matrix4f cameraModel = this->GetModelTransformMatrix ();

	cameraModel[0][3] = - cameraModel[0][3];
	cameraModel[1][3] = - cameraModel[1][3];
	cameraModel[2][3] = - cameraModel[2][3];

	return cameraModel;
}


