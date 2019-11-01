#ifndef _ROTATINGOBJECT_H_
#define _ROTATINGOBJECT_H_

#include "scene/GameObject.h"

class RotatingObject : public GameObject
{
public:
	RotatingObject ();
	~RotatingObject ();

	void OnFrame ();
private:
	
	// rotation speed in degrees per second
	float rotationSpeed;

	// current angle
    float angle;

};

#endif