#include "cilantroengine.h"
#include "Orbiter.h"
#include "util/Timer.h"

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
	GetModelTransform ().RotateBy (0.0f, (1.0f / orbitingPeriod * 360.0f) * Timer::GetFrameDeltaTime () * 10.0f, 0.0f);
}
