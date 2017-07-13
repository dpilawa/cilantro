#include "GameScene.h"

GameScene::GameScene()
{
	gameObjectsCount = 0;
	activeCamera = nullptr;
}

GameScene::~GameScene()
{
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
	gameObject->RegisterCallback ("OnUpdateMeshObject", [&](unsigned int objectHandle) { InvokeCallbacks ("OnUpdateMeshObject", objectHandle); });

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

void GameScene::SetActiveCamera (Camera * camera)
{
	activeCamera = camera;
}

Camera * GameScene::GetActiveCamera () const
{
	return activeCamera;
}


void GameScene::OnModifiedMeshObject (unsigned int objectHandle)
{
	InvokeCallbacks ("OnUpdateMeshObject", objectHandle);
}


