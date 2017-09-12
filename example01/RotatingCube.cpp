#include "RotatingCube.h"

RotatingCube::RotatingCube ()
{
	rotationSpeed = 90.0f;
}


RotatingCube::~RotatingCube ()
{
}

void RotatingCube::OnFrame ()
{
	GetModelTransform ().Rotate (0.0f, rotationSpeed * Time::GetFrameDeltaTime (), 0.0f);
}
