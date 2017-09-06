#include "RotatingCube.h"



RotatingCube::RotatingCube ()
{
}


RotatingCube::~RotatingCube ()
{
}

void RotatingCube::OnFrame ()
{
	GetModelTransform ().Rotate (0.0f, 0.25f, 0.0f);
}
