#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Transform.h"
#include "Renderer.h"

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

	// methods invoked by renderer
	virtual void OnDraw (Renderer& renderer);

	// get transformation object's reference
	Transform& getWorldTransform ();

};

#endif
