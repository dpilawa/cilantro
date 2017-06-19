#include "GameScene.h"

GameScene::GameScene()
{
	gameObjectsCount = 0;
	myGameLoop = nullptr;
}

GameScene::~GameScene()
{
}

void GameScene::SetGameLoop (GameLoop & gameloop)
{
	myGameLoop = &gameloop;
}

GameObject& GameScene::AddGameObject (GameObject* gameObject)
{
	// set object's handle
	gameObject->SetHandle (gameObjectsCount++);

	// set object's pointer to scene
	gameObject->SetGameScene (*this);

	// insert into collection
	gameObjects.push_back (gameObject);

	// set callback on object modification
	gameObject->RegisterCallback ("OnUpdateMeshObject", std::bind (&GameScene::OnModifiedMeshObject, this, std::placeholders::_1));

	// return object
	return *gameObject;
}


std::vector<GameObject*>& GameScene::GetGameObjects ()
{
	return gameObjects;
}

unsigned int GameScene::getGameObjectsCount () const
{
	return gameObjectsCount;
}


void GameScene::OnModifiedMeshObject (unsigned int objectHandle)
{
	InvokeCallbacks ("OnUpdateMeshObject", objectHandle);
}


