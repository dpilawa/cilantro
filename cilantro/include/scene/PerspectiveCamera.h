#ifndef _PERSPECTIVECAMERA_H_
#define _PERSPECTIVECAMERA_H_

#include "cilantroengine.h"
#include "scene/Camera.h"
#include "math/Matrix4f.h"

namespace cilantro {

class __CEAPI PerspectiveCamera : public Camera
{
public:
    __EAPI PerspectiveCamera (std::shared_ptr<GameScene> gameScene, float fov, float near, float far);
    __EAPI ~PerspectiveCamera ();

    __EAPI Matrix4f GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const;

private:

    float m_cameraFOV;
    float m_nearPlane;
    float m_farPlane;
};

} // namespace cilantro

#endif

