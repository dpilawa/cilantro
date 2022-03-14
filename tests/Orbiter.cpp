#include "cilantroengine.h"
#include "scene/GameScene.h"
#include "Orbiter.h"
#include "math/Mathf.h"
#include "math/Quaternion.h"
#include "system/Timer.h"
#include "system/Game.h"
#include <cmath>

Orbiter::Orbiter (CGameScene* gameScene, GameObject& parent, float axisPeriod, float axisAngle, float orbitingPeriod, float orbitingDistance, float orbitInclination) : GameObject (gameScene), parent (parent)
{
    this->orbitingPeriod = orbitingPeriod;
    this->axisPeriod = axisPeriod;
    this->axisAngle = axisAngle;    
    this->orbitingDistance = orbitingDistance;
    this->orbitInclination = orbitInclination;
    
    this->currentOrbitRotation = 0.0f;
    this->currentAxisRotation = 0.0f;

    this->daysPerSecond = 0.5f;

    tilt = Mathf::GenRotationQuaternion (Vector3f (0.0f, 0.0f, 1.0f), Mathf::Deg2Rad (axisAngle));
}

Orbiter::~Orbiter ()
{
}

void Orbiter::OnFrame ()
{
    currentOrbitRotation = std::fmod (currentOrbitRotation + (1.0f / orbitingPeriod * 360.0f) * gameScene->GetTimer ()->GetFrameDeltaTime () * daysPerSecond, 360.0f);
    currentAxisRotation = std::fmod (currentAxisRotation + (1.0f / axisPeriod * 360.0f) * gameScene->GetTimer ()->GetFrameDeltaTime () * daysPerSecond, 360.0f);

    Quaternion axis = Mathf::GenRotationQuaternion (Vector3f(0.0f, 1.0f, 0.0f), Mathf::Deg2Rad (currentAxisRotation));
    Quaternion q = Mathf::Product (tilt, axis);

    Quaternion orbit = Mathf::GenRotationQuaternion (Vector3f(0.0f, 1.0f, 0.0f), Mathf::Deg2Rad (currentOrbitRotation));
    Quaternion inclination = Mathf::GenRotationQuaternion (Vector3f(0.0f, 0.0f, 1.0f), Mathf::Deg2Rad (orbitInclination));
    Vector3f positionInOrbit = Mathf::Rotate (Mathf::Rotate (Vector3f (1.0f, 0.0f, 0.0f) * orbitingDistance, orbit), inclination);

    Vector3f parentPosition = parent.GetPosition ();

    GetLocalTransform ().Rotate (q).Translate (positionInOrbit + parentPosition);
}


