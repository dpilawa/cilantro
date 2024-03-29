#ifndef _PERSPECTIVECAMERA_H_
#define _PERSPECTIVECAMERA_H_

#include "cilantroengine.h"
#include "scene/Camera.h"
#include "math/Matrix4f.h"

class PerspectiveCamera : public Camera
{
public:
    __EAPI PerspectiveCamera (CGameScene* gameScene, float fov, float near, float far);
    __EAPI ~PerspectiveCamera ();

    __EAPI Matrix4f GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const;

private:

    float cameraFOV;
    float nearPlane;
    float farPlane;
};

#endif

