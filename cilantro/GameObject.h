#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Transform.h"
#include "CallbackProvider.h"
#include "Renderer.h"
#include <vector>
#include <string>
#include <functional>

class GameObject : public CallbackProvider<std::string, unsigned int>
{
public:
	GameObject ();
	~GameObject ();

	// handle  operations
	void SetHandle (unsigned int handle);
	unsigned int GetHandle () const;

	// set pointer to parent object (i.e. put current object inside hierarchy)
	void SetParentObject (GameObject & parent);

	// set pointer to game scene
	void SetGameScene (GameScene & scene);

	// invoked by game loop during initializaton
	virtual void OnStart ();

	// invoked by game loop on each frame
	virtual void OnFrame ();
	virtual void OnDraw (Renderer& renderer);

	// invoked by game loop during deinitialization
	virtual void OnEnd ();

	// get transformation object's reference
	// this function is recursive and concatenates world-space 
	// transformation matrices of parent objects 
	Transform& GetModelTransform ();

	// get transformatio matrix
	Matrix4f GetModelTransformMatrix ();

private:
	// object's handle (index in vector inside GameScene)
	unsigned int objectHandle;

	// pointer to parent object (objects may form a hierarchy)
	GameObject* parentObject;

	// pointer to a game scene
	GameScene* myGameScene;

	// object's transformation from local space to world space
	Transform modelTransform;
};

#endif
