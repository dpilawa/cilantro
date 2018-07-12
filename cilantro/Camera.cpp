#include "cilantroengine.h"
#include "GameObject.h"
#include "Camera.h"
#include "Mathf.h"
#include "Matrix4f.h"

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


