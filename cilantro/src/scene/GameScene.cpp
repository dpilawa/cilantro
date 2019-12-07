#include "cilantroengine.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/Camera.h"
#include "scene/Material.h"
#include "util/CallbackProvider.h"
#include "util/LogMessage.h"

#include <vector>

GameScene::GameScene(GameLoop* gameLoop)
{
	this->gameLoop = gameLoop;

	this->gameObjectsCount = 0;
	this->activeCamera = nullptr;
}

GameScene::~GameScene()
{
}

GameObject& GameScene::AddGameObject (GameObject* gameObject)
{
	// set object's handle
    unsigned int handle = gameObjectsCount++;
    gameObject->SetHandle (handle);

	// set game loop reference
	gameObject->SetGameLoop (this->gameLoop);

    // insert into collection
    gameObjects[handle] = gameObject;

    // set callbacks on object modification
	// this is just a passthrough of callbacks to subscribers (e.g. Renderer)
	gameObject->RegisterCallback ("OnUpdateMeshObject", [ & ](unsigned int objectHandle) { InvokeCallbacks ("OnUpdateMeshObject", objectHandle); });
	gameObject->RegisterCallback ("OnUpdateLight", [ & ](unsigned int objectHandle) { InvokeCallbacks ("OnUpdateLight", objectHandle); });
	gameObject->RegisterCallback ("OnUpdateSceneGraph", [ & ](unsigned int objectHandle) { InvokeCallbacks ("OnUpdateSceneGraph", objectHandle); });
    gameObject->RegisterCallback ("OnUpdateTransform", [&](unsigned int objectHandle) { 
		InvokeCallbacks ("OnUpdateTransform", objectHandle);
        this->GetObjectByHandle (objectHandle).CalculateModelTransformMatrix ();
    });

    // return object
	return *gameObject;
}

GameObject& GameScene::GetObjectByHandle (unsigned int handle)
{
    auto find = gameObjects.find (handle);

	if (find == gameObjects.end ())
	{
        LogMessage (__func__, EXIT_FAILURE) << "Unable to find object by handle" << handle;
    }

    return *(find->second);
}

Material& GameScene::AddMaterial (Material* material)
{
	// insert into collection
	materials.push_back (material);

	// return material
	return *material;
}

std::unordered_map<unsigned int, GameObject*>& GameScene::GetGameObjects ()
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


