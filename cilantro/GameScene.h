#ifndef _GAMESCENE_H_
#define _GAMESCENE_H_

#include <functional>
#include <vector>
#include "GameObject.h"
#include "LogMessage.h"

// This class represents a game world (a.k.a scene or level)
// It contains all visible and invisible objects in a game
class GameScene : public CallbackProvider<size_t>
{
public:
	GameScene();
	~GameScene();

	// add GameObject to a scene
	// returns reference to that added object
	GameObject& AddGameObject (GameObject* gameObject);

	// return reference to GameObject vector
	std::vector <GameObject*>& GetGameObjects ();

private:
	// vector of all GameObjects in the scene
	std::vector <GameObject*> gameObjects;

	// callback function to get notified about changed GameObjects
	void OnModifiedGameObject (size_t objectHandle);

};

#endif

