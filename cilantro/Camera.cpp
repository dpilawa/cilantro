#include "Camera.h"

Camera::Camera ()
{
}

Camera::~Camera ()
{
}

Matrix4f Camera::GetViewMatrix ()
{
	return Mathf::Invert (GetModelTransformMatrix ());
}


