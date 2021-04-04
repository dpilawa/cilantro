#include "cilantroengine.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/Camera.h"
#include "scene/Material.h"
#include "scene/PBRMaterial.h"
#include "scene/PhongMaterial.h"
#include "system/CallbackProvider.h"
#include "system/LogMessage.h"

#include <vector>

GameScene::GameScene()
{
    this->activeCamera = nullptr;
}

GameScene::~GameScene()
{
}

ResourceManager<GameObject>& GameScene::GetGameObjectManager ()
{
    return gameObjects;
}

ResourceManager<Material>& GameScene::GetMaterialManager ()
{
    return materials;
}

void GameScene::SetActiveCamera (const std::string& name)
{
    activeCamera = &(gameObjects.GetByName<Camera> (name));
}

Camera* GameScene::GetActiveCamera () const
{
    return activeCamera;
}

