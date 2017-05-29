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
	// insert into collection
	gameObjects.push_back (gameObject);
	// notify all callback subscribers about new object
	for each (auto callback in objectCallbacks)
	{
		callback (gameObject);
	}
	// return object
	return *gameObject;
}

// return reference to GameObject vector
std::vector<GameObject*>& GameScene::GetGameObjects ()
{
	return gameObjects;
}

// add new GameObject callback
void GameScene::AddGameObjectCallBack (std::function<void (GameObject*)> callback)
{
	objectCallbacks.push_back (callback);
}


