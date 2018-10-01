#include "cilantroengine.h"
#include "RotatingObject.h"
#include "Time.h"

RotatingObject::RotatingObject ()
{
	rotationSpeed = 90.0f;
}


RotatingObject::~RotatingObject ()
{
}

void RotatingObject::OnFrame ()
{
	GetModelTransform ().Rotate (0.0f, rotationSpeed * Time::GetFrameDeltaTime (), 0.0f);
}
