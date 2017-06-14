#include "GameScene.h"

GameScene::GameScene()
{
	gameObjectsCount = 0;
}

GameScene::~GameScene()
{
}

GameObject& GameScene::AddGameObject (GameObject* gameObject)
{
	// set object handle
	gameObject->SetHandle (gameObjectsCount++);
	// insert into collection
	gameObjects.push_back (gameObject);
	// set callback on object modification
	gameObject->RegisterCallback (std::bind (&GameScene::OnModifiedGameObject, this, std::placeholders::_1));
	// notify subscribers about new object
	OnModifiedGameObject (gameObject->GetHandle ());
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


void GameScene::OnModifiedGameObject (unsigned int objectHandle)
{
	InvokeCallbacks (objectHandle);
}


