#ifndef _ORBITER_H_
#define _ORBITER_H_

#include "GameObject.h"

class Orbiter : public GameObject
{
public:
	Orbiter ();
	Orbiter (float period);
	~Orbiter ();

	void OnFrame ();
private:

	// orbiting period in days
	float orbitingPeriod;
};

#endif

