#include "GameScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

GameObject& GameScene::AddGameObject (GameObject* gameObject)
{
	// set object handle
	gameObject->SetHandle (gameObjects.size ());
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


void GameScene::OnModifiedGameObject (size_t objectHandle)
{
	InvokeCallbacks (objectHandle);
}


