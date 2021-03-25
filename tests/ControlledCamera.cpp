#include "ControlledCamera.h"
#include "input/Input.h"
#include "input/GLFWInputController.h"
#include "system/EngineContext.h"

ControlledCamera::ControlledCamera (float fov, float near, float far, float sensitivity) : PerspectiveCamera (fov, near, far), sensitivity (sensitivity)
{
}

ControlledCamera::~ControlledCamera()
{
}

void ControlledCamera::Initialize ()
{
    GLFWInputController& c = dynamic_cast<GLFWInputController&> (EngineContext::GetInputController ());

    c.CreateInputAxis ("moveforward", InputKey::KeyW, {}, 1.0f);
    c.CreateInputAxis ("moveforward", InputKey::KeyS, {}, -1.0f);	

    c.CreateInputAxis ("moveright", InputKey::KeyD, {}, 1.0f);
    c.CreateInputAxis ("moveright", InputKey::KeyA, {}, -1.0f);

    c.CreateInputAxis ("camerapitch", InputAxis::MouseY, 1.0f);
    c.CreateInputAxis ("camerayaw", InputAxis::MouseX, 1.0f);

    c.BindInputAxis ("moveright", [&](float a) { MoveRight (a); });
    c.BindInputAxis ("moveforward", [&](float a) { MoveForward (a); });
    c.BindInputAxis ("camerapitch", [&](float a) { PitchBy (a); });
    c.BindInputAxis ("camerayaw", [&](float a) { YawBy (a); });
}

void ControlledCamera::MoveRight (float offset)
{
    GetModelTransform ().TranslateBy (GetRight () * offset * sensitivity);
}

void ControlledCamera::MoveForward (float offset)
{
    GetModelTransform ().TranslateBy (-GetForward () * offset * sensitivity);
}

void ControlledCamera::YawBy (float angle)
{
    GetModelTransform ().RotateBy (0.0f, angle * sensitivity, 0.0f);
}

void ControlledCamera::PitchBy (float angle)
{
    GetModelTransform ().RotateBy (angle * sensitivity, 0.0f, 0.0f);
}