#include "cilantroengine.h"
#include "scene/PerspectiveCamera.h"
#include "scene/Camera.h"
#include "math/Matrix4f.h"
#include "math/Mathf.h"

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



