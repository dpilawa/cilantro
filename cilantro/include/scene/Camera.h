#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "math/Matrix4f.h"

namespace cilantro {

class Camera : public GameObject
{
public:
    Camera (GameScene* gameScene);
    virtual ~Camera ();

    Matrix4f GetViewMatrix ();
    virtual Matrix4f GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const = 0;

};

} // namespace cilantro

#endif

