#include "ControlledCamera.h"
#include "input/Input.h"
#include "input/GLFWInputController.h"
#include "system/Game.h"

using namespace cilantro;

ControlledCamera::ControlledCamera (std::shared_ptr<GameScene> gameScene, float fov, float near, float far, float speed, float sensitivity) : PerspectiveCamera (gameScene, fov, near, far), speed (speed), sensitivity (sensitivity)
{
}

ControlledCamera::~ControlledCamera()
{
}

std::shared_ptr<ControlledCamera> ControlledCamera::Initialize ()
{
    auto c = std::dynamic_pointer_cast<GLFWInputController> (GetGameScene ()->GetGame ()->GetInputController ());

    c->CreateInputAxis ("moveforward", EInputKey::KeyW, {}, 1.0f);
    c->CreateInputAxis ("moveforward", EInputKey::KeyS, {}, -1.0f);	

    c->CreateInputAxis ("moveright", EInputKey::KeyD, {}, 1.0f);
    c->CreateInputAxis ("moveright", EInputKey::KeyA, {}, -1.0f);

    c->CreateInputAxis ("camerapitch", EInputAxis::MouseY, 1.0f);
    c->CreateInputAxis ("camerayaw", EInputAxis::MouseX, 1.0f);

    c->BindInputAxis ("moveright", [&](float a) { MoveRight (a); });
    c->BindInputAxis ("moveforward", [&](float a) { MoveForward (a); });
    c->BindInputAxis ("camerapitch", [&](float a) { PitchBy (a); });
    c->BindInputAxis ("camerayaw", [&](float a) { YawBy (a); });

    return std::dynamic_pointer_cast<ControlledCamera> (shared_from_this ());
}

void ControlledCamera::MoveRight (float offset)
{
    GetModelTransform ()->TranslateBy (GetRight () * offset * speed);
}

void ControlledCamera::MoveForward (float offset)
{
    GetModelTransform ()->TranslateBy (-GetForward () * offset * speed);
}

void ControlledCamera::YawBy (float angle)
{
    GetModelTransform ()->RotateBy (0.0f, angle * sensitivity, 0.0f);
}

void ControlledCamera::PitchBy (float angle)
{
    GetModelTransform ()->RotateBy (angle * sensitivity, 0.0f, 0.0f);
}