#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "scene/Camera.h"
#include "math/Mathf.h"
#include "math/Matrix4f.h"

namespace cilantro {

Camera::Camera (CGameScene* gameScene) : GameObject (gameScene)
{
}

Camera::~Camera ()
{
}

Matrix4f Camera::GetViewMatrix ()
{
    return Mathf::Invert (GetModelTransformMatrix ());
}

} // namespace cilantro


