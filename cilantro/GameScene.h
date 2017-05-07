#ifndef _GAMESCENE_H_
#define _GAMESCENE_H_

#include <vector>
#include "GameObject.h"
#include "GLRenderer.h"

class GameScene
{
	std::vector<GameObject*> gameObjects;
	Renderer myRenderer;

public:
	GameScene();
	~GameScene();
	void Run ();
	int AddGameObject (GameObject* object);
};

#endif

