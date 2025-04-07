#ifndef _ORTHOGRAPHICCAMERA_H_
#define _ORTHOGRAPHICCAMERA_H_

#include "cilantroengine.h"
#include "scene/Camera.h"
#include "math/Matrix4f.h"

namespace cilantro {

class __CEAPI OrthographicCamera : public Camera
{
public:
    __EAPI OrthographicCamera (std::shared_ptr<GameScene> gameScene, float width, float near, float far);
    __EAPI virtual ~OrthographicCamera ();

    __EAPI Matrix4f GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const;

private:

    float m_cameraWidth;
    float m_nearPlane;
    float m_farPlane;
};

} // namespace cilantro

#endif