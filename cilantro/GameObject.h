#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Transform.h"
#include "CallbackProvider.h"
#include <vector>
#include <functional>

class GameObject : public CallbackProvider<size_t>
{
public:
	GameObject ();
	~GameObject ();

	// getters and setters
	void SetHandle (size_t handle);
	size_t GetHandle () const;

	// invoked by game loop during initializaton
	virtual void OnStart ();
	// invoked by game loop on each frame
	virtual void OnFrame ();
	// invoked by game loop during deinitialization
	virtual void OnEnd ();

	// get transformation object's reference
	Transform& getWorldTransform ();
private:
	// object's handle (index in vector inside GameScene)
	size_t objectHandle;

	// object's transformation from local space to world space
	Transform worldTransform;
};

#endif
