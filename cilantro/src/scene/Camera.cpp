#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "scene/Camera.h"
#include "math/Mathf.h"
#include "math/Matrix4f.h"
#include "math/Vector4f.h"
#include <array>

namespace cilantro {

Camera::Camera (std::shared_ptr<GameScene> gameScene) : GameObject (gameScene)
{
}

Camera::~Camera ()
{
}

Matrix4f Camera::GetViewMatrix () const
{
    return Mathf::Invert (GetWorldTransformMatrix ());
}

std::array<Vector3f, 8> Camera::GetFrustumVertices (unsigned int xRes, unsigned int yRes) const
{
    Matrix4f invMV = Mathf::Invert (GetProjectionMatrix (xRes, yRes) * GetViewMatrix ());
    std::array<Vector3f, 8> frustumVertices;
    
    // calculate camera frustum vertices in world space
    for (size_t x = 0; x < 2; ++x)
    {
        for (size_t y = 0; y < 2; ++y)
        {
            for (size_t z = 0; z < 2; ++z)
            {
                Vector4f v = invMV * Vector4f {2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f};
                frustumVertices[x * 4 + y * 2 + z] = v / v[3];
            }
        }
    }

    return frustumVertices;
}

} // namespace cilantro


