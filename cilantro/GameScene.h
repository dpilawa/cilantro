#ifndef _GAMESCENE_H_
#define _GAMESCENE_H_

#include <functional>
#include <vector>
#include "GameObject.h"
#include "LogMessage.h"

// This class represents a game world (a.k.a scene or level)
// It contains all visible and invisible objects in a game
class GameScene
{
public:
	GameScene();
	~GameScene();

	// add game object
	GameObject& AddGameObject (GameObject* gameObject);

	// return reference to GameObject vector
	std::vector <GameObject*>& GetGameObjects ();

	// The callback is used to notify subscribers of new or modified GameObjects
	// (e.g. Renderer subscribes to GameObject changes which need to be loaded into GPU memory). 
	// Subscriber must set this callback to one of its functions.
	void RegisterGameObjectModifiedCallback (std::function<void (size_t)> callback);

	// notify subscribers of the above callback about object changes
	void OnModifiedGameObject (size_t objectHandle);

private:
	// vector of all GameObjects in the scene
	std::vector <GameObject*> gameObjects;
	// vector of callbacks for new or modified GameObjects
	std::vector <std::function<void (size_t)>> objectCallbacks;
};

#endif

