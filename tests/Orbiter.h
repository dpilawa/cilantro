#ifndef _ORBITER_H_
#define _ORBITER_H_

#include "scene/GameObject.h"
#include "math/Vector3f.h"

class Orbiter : public GameObject
{
public:
	Orbiter (float period, float distance, float angle);
	~Orbiter ();

	void OnFrame ();

private:

    // orbit game object
    GameObject* orbit;

    // axis and plane of the orbit
    Vector3f orbitAxis;
    Vector3f orbitPlane;

    // orbiting period in days
	float orbitingPeriod;
	
	// orbiting distance
	float orbitingDistance;

	// axis angle
    float orbitAxisAngle;

	// current orbit rotation value in degrees
    float orbitRotation;
};

#endif

