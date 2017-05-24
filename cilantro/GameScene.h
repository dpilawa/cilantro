#ifndef _GAMESCENE_H_
#define _GAMESCENE_H_

#include <vector>
#include "GameObject.h"

// This class represents a game world (a.k.a scene or level)
// It contains all visible and invisible objects in a game
class GameScene
{
	std::vector <GameObject*> gameObjects;

public:
	GameScene();
	~GameScene();

	GameObject& AddGameObject (GameObject* object);
	std::vector <GameObject*> & GetGameObjectsVector ();
};

#endif

