#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Transform.h"

class GameObject
{
	// object's transformation from local space to world space
	Transform objectTransform;

public:
	GameObject ();
	~GameObject ();

	// get transformation object's reference
	Transform& getTransform ();

};

#endif
