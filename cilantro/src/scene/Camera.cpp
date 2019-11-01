#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "scene/Camera.h"
#include "math/Mathf.h"
#include "math/Matrix4f.h"

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


