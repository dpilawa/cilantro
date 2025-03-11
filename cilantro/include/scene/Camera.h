#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "math/Matrix4f.h"

namespace cilantro {

class __CEAPI Camera : public GameObject
{
public:
    __EAPI Camera (GameScene* gameScene);
    __EAPI virtual ~Camera ();

    __EAPI Matrix4f GetViewMatrix ();
    __EAPI virtual Matrix4f GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const = 0;

};

} // namespace cilantro

#endif

