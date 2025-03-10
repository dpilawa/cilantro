#include "cilantroengine.h"
#include "scene/OrthographicCamera.h"
#include "scene/Camera.h"
#include "math/Matrix4f.h"
#include "math/Mathf.h"

namespace cilantro
{

OrthographicCamera::OrthographicCamera (CGameScene* gameScene, float width, float near, float far) :
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

} // namespace cilantro

