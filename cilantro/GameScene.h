#ifndef _GAMESCENE_H_
#define _GAMESCENE_H_

#include <functional>
#include <vector>
#include "GameObject.h"
#include "LogMessage.h"

class GameLoop;

// This class represents a game world (a.k.a scene or level)
// It contains all visible and invisible objects in a game
class GameScene : public CallbackProvider<std::string, unsigned int>
{
public:
	GameScene();
	~GameScene();

	// set game loop pointer
	void SetGameLoop (GameLoop& gameloop);

	// add GameObject to a scene
	// returns reference to that added object
	GameObject& AddGameObject (GameObject* gameObject);

	// return reference to GameObject vector
	std::vector <GameObject*>& GetGameObjects ();

	// return number of scene's GameObjects
	unsigned int getGameObjectsCount () const;

private:
	// pointer to a game loop
	GameLoop* myGameLoop;
	
	// vector of all GameObjects in the scene
	std::vector <GameObject*> gameObjects;

	// count of all GameObjects in the scene
	unsigned int gameObjectsCount;

	// callback function to get notified about changed MeshObjects
	void OnModifiedMeshObject (unsigned int objectHandle);

};

#endif

