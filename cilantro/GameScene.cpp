#include "cilantroengine.h"
#include "GameScene.h"
#include "GameObject.h"
#include "Camera.h"
#include "CallbackProvider.h"
#include "Material.h"
#include <vector>

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

	// attach to game
    gameObject->AttachToGame (gameLoop);

    // insert into collection
	gameObjects.push_back (gameObject);

	// set callbacks on object modification
	// this is just a passthrough of callbacks to subscribers (e.g. Renderer)
	gameObject->RegisterCallback ("OnUpdateMeshObject", [ & ](unsigned int objectHandle) { InvokeCallbacks ("OnUpdateMeshObject", objectHandle); });
	gameObject->RegisterCallback ("OnUpdateLight", [ & ](unsigned int objectHandle) { InvokeCallbacks ("OnUpdateLight", objectHandle); });
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

void GameScene::SetActiveCamera (Camera* camera)
{
	activeCamera = camera;
}

Camera* GameScene::GetActiveCamera () const
{
	return activeCamera;
}


