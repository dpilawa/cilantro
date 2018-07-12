#include "cilantroengine.h"
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
	GetModelTransform ().Rotate (0.0f, (1.0f / orbitingPeriod * 360.0f) * Time::GetFrameDeltaTime (), 0.0f);
}
