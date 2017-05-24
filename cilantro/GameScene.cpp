#include "GameScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

// add GameObject to a scene
// returns reference to that added pointer
GameObject& GameScene::AddGameObject (GameObject* gameObject)
{
	gameObjects.push_back (gameObject);
	return *gameObject;
}

