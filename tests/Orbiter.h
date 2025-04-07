#ifndef _ORBITER_H_
#define _ORBITER_H_

#include "scene/GameObject.h"
#include "math/Vector3f.h"

using namespace cilantro;

class Orbiter : public GameObject
{
public:
    Orbiter (std::shared_ptr<GameScene> gameScene, std::shared_ptr<GameObject> parent, float axisPeriod, float axisAngle, float orbitingPeriod, float orbitingDistance, float orbitInclination);
    ~Orbiter ();

    void OnFrame ();

private:

    std::weak_ptr<GameObject> parent;

    // axis and plane of the orbit
    Quaternion tilt;

    // orbit parameters
    float axisPeriod;    
    float axisAngle;
    float orbitingPeriod;
    float orbitingDistance;
    float orbitInclination;

    // current orbit and axis rotation value in degrees
    float currentOrbitRotation;
    float currentAxisRotation;

    // speed
    float daysPerSecond;
};

#endif

