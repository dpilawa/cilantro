#include "cilantroengine.h"
#include "RotatingObject.h"
#include "Timer.h"

RotatingObject::RotatingObject ()
{
	rotationSpeed = 90.0f;
}


RotatingObject::~RotatingObject ()
{
}

void RotatingObject::OnFrame ()
{
	GetModelTransform ().Rotate (0.0f, rotationSpeed * Timer::GetFrameDeltaTime (), 0.0f);
}