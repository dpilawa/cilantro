#include "Orbiter.h"

Orbiter::Orbiter ()
{
	orbitingPeriod = 1.0f;
}


Orbiter::Orbiter (float period)
{
	orbitingPeriod = period;
}

Orbiter::~Orbiter ()
{
}

void Orbiter::OnFrame ()
{
	GetModelTransform ().Rotate (0.0f, orbitingPeriod * Time::GetFrameDeltaTime () / 360.0f, 0.0f);
}
