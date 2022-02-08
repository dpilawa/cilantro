#include "cilantroengine.h"
#include "scene/OrthographicCamera.h"
#include "scene/Camera.h"
#include "math/Matrix4f.h"
#include "math/Mathf.h"

OrthographicCamera::OrthographicCamera (GameScene* gameScene, float width, float near, float far) :
    Camera (gameScene),
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



