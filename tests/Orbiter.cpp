#include "cilantroengine.h"
#include "scene/GameScene.h"
#include "Orbiter.h"
#include "math/Mathf.h"
#include "math/Quaternion.h"
#include "util/Timer.h"
#include <cmath>

Orbiter::Orbiter (float period, float distance, float angle)
{
	orbitingPeriod = period;
    orbitingDistance = distance;
    orbitAxisAngle = angle;
    orbitRotation = 0.0f;

    Quaternion q = Mathf::GenRotationQuaternion (Vector3f (0.0f, 0.0f, 1.0f), Mathf::Deg2Rad (orbitAxisAngle));
    orbitAxis = Mathf::Rotate (Vector3f (0.0f, 1.0f, 0.0f), q);
    orbitPlane = Mathf::Rotate (Vector3f (orbitingDistance, 0.0f, 0.0f), q);
}

Orbiter::~Orbiter ()
{
}

void Orbiter::OnFrame ()
{
    orbitRotation = std::fmod (orbitRotation + (1.0f / orbitingPeriod * 360.0f) * Timer::GetFrameDeltaTime () * 10.0f, 360.0f);
    Quaternion q = Mathf::GenRotationQuaternion (orbitAxis, Mathf::Deg2Rad (orbitRotation));

    GetModelTransform ().Translate (Mathf::Rotate (orbitPlane, q));
}


