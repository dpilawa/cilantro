#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "cilantroengine.h"
#include "CallbackProvider.h"
#include "Transform.h"
#include <string>

class GameScene;
class Renderer;

class GameObject : public CallbackProvider<std::string, unsigned int>
{
public:
	__EAPI GameObject ();
	__EAPI virtual ~GameObject ();

	// handle  operations
	__EAPI void SetHandle (unsigned int handle);
	__EAPI unsigned int GetHandle () const;

	// set pointer to parent object (i.e. put current object inside hierarchy)
	__EAPI void SetParentObject (GameObject & parent);

	// set pointer to game scene
	__EAPI void SetGameScene (GameScene & scene);

	// invoked by game loop during initializaton
	__EAPI virtual void OnStart ();

	// invoked by game loop on each frame
	__EAPI virtual void OnFrame ();
	__EAPI virtual void OnDraw (Renderer& renderer);

	// invoked by game loop during deinitialization
	__EAPI virtual void OnEnd ();

	// get transformation object's reference
	// this function is recursive and concatenates world-space
	// transformation matrices of parent objects
	__EAPI Transform& GetModelTransform ();

	// get transformation matrices
	__EAPI Matrix4f GetModelTransformMatrix ();

	// get object origin's position in world space
	__EAPI Vector4f GetPosition ();

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
