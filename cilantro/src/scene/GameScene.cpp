#include "cilantroengine.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/Camera.h"
#include "scene/Material.h"
#include "system/CallbackProvider.h"
#include "system/LogMessage.h"

#include <vector>

GameScene::GameScene()
{
    this->gameObjectsCount = 0;
    this->materialsCount = 0;
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
    gameObject->AttachToGame (this->game);

    // insert into collection
    gameObjects[handle] = gameObject;

    // set callbacks on object modification
    // this is just a passthrough of callbacks to subscribers (e.g. Renderer)
    gameObject->RegisterCallback ("OnUpdateMeshObject", [ & ](unsigned int objectHandle, unsigned int) { InvokeCallbacks ("OnUpdateMeshObject", objectHandle, 0); });
    gameObject->RegisterCallback ("OnUpdateLight", [ & ](unsigned int objectHandle, unsigned int) { InvokeCallbacks ("OnUpdateLight", objectHandle, 0); });
    gameObject->RegisterCallback ("OnUpdateSceneGraph", [ & ](unsigned int objectHandle, unsigned int) { InvokeCallbacks ("OnUpdateSceneGraph", objectHandle, 0); });
    gameObject->RegisterCallback ("OnUpdateTransform", [&](unsigned int objectHandle, unsigned int) { 
        InvokeCallbacks ("OnUpdateTransform", objectHandle, 0);
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
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unable to find object by handle" << handle;
    }

    return *(find->second);
}

Material& GameScene::AddMaterial (Material* material)
{
    // set material's handle
    unsigned int handle = materialsCount++;
    material->SetHandle (handle);

    // insert into collection
    materials[handle] = material;

    // register callbacks
    material->RegisterCallback ("OnUpdateMaterial", [ & ](unsigned int materialHandle, unsigned int textureUnit) { InvokeCallbacks ("OnUpdateMaterial", materialHandle, textureUnit); });

    // return object
    return *material;
}

std::unordered_map<unsigned int, GameObject*>& GameScene::GetGameObjects ()
{
    return gameObjects;
}

std::unordered_map<unsigned int, Material*>& GameScene::GetMaterials ()
{
    return materials;
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


