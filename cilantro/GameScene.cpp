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
	// set object handle
	gameObject->SetHandle (gameObjects.size ());
	// insert into collection
	gameObjects.push_back (gameObject);
	// set callback on object modification
	gameObject->SetGameObjectModifiedCallback (std::bind (&GameScene::OnModifiedGameObject, this, std::placeholders::_1));
	// notify subscribers
	OnModifiedGameObject (gameObject->GetHandle ());
	// return object
	return *gameObject;
}

// return reference to GameObject vector
std::vector<GameObject*>& GameScene::GetGameObjects ()
{
	return gameObjects;
}

void GameScene::RegisterGameObjectModifiedCallback (std::function<void (size_t)> callback)
{
	objectCallbacks.push_back (callback);
}

void GameScene::OnModifiedGameObject (size_t objectHandle)
{
	LogMessage () << "GameScene notifying on new or updated object [" << objectHandle << "]";
	// notify all callback subscribers about new or modified object
	for each (auto callback in objectCallbacks)
	{
		callback (objectHandle);
	}
}


