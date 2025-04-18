#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "scene/Camera.h"
#include "math/Mathf.h"
#include "math/Matrix4f.h"

namespace cilantro {

Camera::Camera (std::shared_ptr<GameScene> gameScene) : GameObject (gameScene)
{
}

Camera::~Camera ()
{
}

Matrix4f Camera::GetViewMatrix ()
{
    return Mathf::Invert (GetWorldTransformMatrix ());
}

} // namespace cilantro


