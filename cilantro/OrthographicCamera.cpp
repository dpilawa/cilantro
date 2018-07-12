#include "cilantroengine.h"
#include "OrthographicCamera.h"
#include "Camera.h"
#include "Matrix4f.h"
#include "Mathf.h"

OrthographicCamera::OrthographicCamera (float width, float near, float far) :
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



