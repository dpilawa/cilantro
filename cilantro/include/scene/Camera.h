#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "math/Matrix4f.h"
#include "math/Vector4f.h"
#include <array>

namespace cilantro {

class __CEAPI Camera : public GameObject
{
public:
    __EAPI Camera (std::shared_ptr<GameScene> gameScene);
    __EAPI virtual ~Camera ();

    __EAPI Matrix4f GetViewMatrix () const;
    __EAPI virtual Matrix4f GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const = 0;

    __EAPI std::array<Vector3f, 8> GetFrustumVertices (unsigned int xRes, unsigned int yRes) const;

};

} // namespace cilantro

#endif

