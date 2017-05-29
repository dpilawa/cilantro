#ifndef _GAMESCENE_H_
#define _GAMESCENE_H_

#include <functional>
#include <vector>
#include "GameObject.h"

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

	// add new GameObject callback
	void AddGameObjectCallBack (std::function<void (GameObject*)> callback);

private:
	// vector of all GameObjects in the scene
	std::vector <GameObject*> gameObjects;
	// vector of callbacks for new GameObjects
	std::vector <std::function<void (GameObject*)>> objectCallbacks;
};

#endif

