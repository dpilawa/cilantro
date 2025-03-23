#include "cilantroengine.h"
#include "scene/PerspectiveCamera.h"
#include "scene/Camera.h"
#include "math/Matrix4f.h"
#include "math/Mathf.h"

namespace cilantro
{

PerspectiveCamera::PerspectiveCamera (std::shared_ptr<GameScene> gameScene, float fov, float near, float far) :
    Camera (gameScene),
    m_cameraFOV (fov), m_nearPlane (near), m_farPlane (far)
{
}

PerspectiveCamera::~PerspectiveCamera ()
{
}

Matrix4f PerspectiveCamera::GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const
{
    return Mathf::GenPerspectiveProjectionMatrix (float (xRes) / float (yRes), Mathf::Deg2Rad (m_cameraFOV), m_nearPlane, m_farPlane);
}

} // namespace cilantro

