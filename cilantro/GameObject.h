#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Transform.h"
#include "CallbackProvider.h"
#include "Renderer.h"
#include <vector>
#include <functional>

class GameObject : public CallbackProvider<unsigned int>
{
public:
	GameObject ();
	~GameObject ();

	// getters and setters
	void SetHandle (unsigned int handle);
	unsigned int GetHandle () const;

	// invoked by game loop during initializaton
	virtual void OnStart ();
	// invoked by game loop on each frame
	virtual void OnFrame ();
	virtual void OnDraw (Renderer& renderer);
	// invoked by game loop during deinitialization
	virtual void OnEnd ();

	// get transformation object's reference
	Transform& getWorldTransform ();
private:
	// object's handle (index in vector inside GameScene)
	unsigned int objectHandle;

	// object's transformation from local space to world space
	Transform worldTransform;
};

#endif
