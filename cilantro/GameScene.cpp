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

	// set callbacks on object modification
	// this is just a passthrough of callbacks to subscribers (e.g. Renderer)
	gameObject->RegisterCallback ("OnUpdateMeshObject", [ & ](unsigned int objectHandle) { InvokeCallbacks ("OnUpdateMeshObject", objectHandle); });
	gameObject->RegisterCallback ("OnUpdatePointLight", [ & ](unsigned int objectHandle) { InvokeCallbacks ("OnUpdatePointLight", objectHandle); });
	gameObject->RegisterCallback ("OnUpdateSceneGraph", [ & ](unsigned int objectHandle) { InvokeCallbacks ("OnUpdateSceneGraph", objectHandle); });
	gameObject->RegisterCallback ("OnUpdateTransform", [ & ](unsigned int objectHandle) { InvokeCallbacks ("OnUpdateTransform", objectHandle); });

	// return object
	return *gameObject;
}

Material& GameScene::AddMaterial (Material* material)
{
	// insert into collection
	materials.push_back (material);

	// return material
	return *material;
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


