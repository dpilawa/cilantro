#ifndef _ORTHOGRAPHICCAMERA_H_
#define _ORTHOGRAPHICCAMERA_H_

#include "cilantroengine.h"
#include "scene/Camera.h"
#include "math/Matrix4f.h"

namespace cilantro {

class __CEAPI OrthographicCamera : public Camera
{
public:
    __EAPI OrthographicCamera (GameScene* gameScene, float width, float near, float far);
    __EAPI ~OrthographicCamera ();

    __EAPI Matrix4f GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const;

private:

    float cameraWidth;
    float nearPlane;
    float farPlane;
};

} // namespace cilantro

#endif