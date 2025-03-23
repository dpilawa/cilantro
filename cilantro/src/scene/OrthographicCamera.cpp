#include "cilantroengine.h"
#include "scene/OrthographicCamera.h"
#include "scene/Camera.h"
#include "math/Matrix4f.h"
#include "math/Mathf.h"

namespace cilantro
{

OrthographicCamera::OrthographicCamera (std::shared_ptr<GameScene> gameScene, float width, float near, float far) :
    Camera (gameScene),
    m_cameraWidth (width), m_nearPlane (near), m_farPlane (far)
{
}

OrthographicCamera::~OrthographicCamera ()
{
}

Matrix4f OrthographicCamera::GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const
{
    return Mathf::GenOrthographicProjectionMatrix (float (xRes) / float (yRes), m_cameraWidth, m_nearPlane, m_farPlane);
}

} // namespace cilantro

