#include "cilantroengine.h"
#include "RotatingObject.h"
#include "util/Timer.h"
#include <cmath>

RotatingObject::RotatingObject ()
{
	rotationSpeed = 90.0f;
    angle = 0.0f;
}

RotatingObject::~RotatingObject ()
{
}

void RotatingObject::OnFrame ()
{
    angle = std::fmod (angle + rotationSpeed * Timer::GetFrameDeltaTime (), 360.0f);

    GetModelTransform ().Rotate (0.0f, angle, 0.0f);
}
