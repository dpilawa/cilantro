#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Transform.h"
#include <vector>
#include <functional>

class GameObject
{
public:
	GameObject ();
	~GameObject ();

	// getters and setters
	void SetHandle (size_t handle);
	size_t GetHandle () const;

	// methods invoked by game loop
	virtual void OnStart ();
	virtual void OnFrame ();
	virtual void OnEnd ();

	// The callback is used to notify subscribers of GameObject modification.
	// Typically this is used to notify the GameScene which in turn notifies the Renderer.
	// Subscriber must set this callback to one of its functions.
	void SetGameObjectModifiedCallback (std::function<void (size_t)> callback);

	// get transformation object's reference
	Transform& getWorldTransform ();
private:
	// object's handle (index in vector inside GameScene)
	size_t objectHandle;
	// vector of callbacks for object modification
	std::vector <std::function<void (size_t)>> objectCallbacks;
	// object's transformation from local space to world space
	Transform worldTransform;
protected:
	// notify callback subscribers
	void GameObjectModifiedNotify ();
};

#endif
