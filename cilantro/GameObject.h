#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Transform.h"

class GameObject
{
	// object's transformation from local space to world space
	Transform worldTransform;

public:
	GameObject ();
	~GameObject ();

	// methods invoked by game loop

	virtual void OnStart ();
	virtual void OnFrame ();
	virtual void OnEnd ();

	virtual void Draw ();

	// get transformation object's reference
	Transform& getWorldTransform ();

};

#endif
