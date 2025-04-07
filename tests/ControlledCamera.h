#ifndef _CONTROLLEDCAMERA_H_
#define _CONTROLLEDCAMERA_H_

#include "scene/PerspectiveCamera.h"

using namespace cilantro;

class ControlledCamera : public PerspectiveCamera
{
public:
    ControlledCamera (std::shared_ptr<GameScene> gameScene, float fov, float near, float far, float speed, float sensitivity);
    virtual ~ControlledCamera ();

    std::shared_ptr<ControlledCamera> Initialize ();

    void MoveRight (float offset);
    void MoveForward (float offset);
    void YawBy (float angle);
    void PitchBy (float angle);

private:
    float speed;
    float sensitivity;
};

#endif