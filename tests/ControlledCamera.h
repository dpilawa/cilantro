#ifndef _CONTROLLEDCAMERA_H_
#define _CONTROLLEDCAMERA_H_

#include "PerspectiveCamera.h"

class ControlledCamera : public PerspectiveCamera
{
public:
    ControlledCamera (float fov, float near, float far, float sensitivity);
    virtual ~ControlledCamera ();

    void Initialize ();

    void MoveRight (float offset);
    void MoveForward (float offset);
    void YawBy (float angle);
    void PitchBy (float angle);

private:
    float sensitivity;
};

#endif