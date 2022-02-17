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
    this->timer = new Timer();
    timer->Tick ();

    this->activeCamera = nullptr;
}

GameScene::~GameScene()
{
    delete timer;

    if (renderer != nullptr)
    {
        renderer->Deinitialize ();
        delete renderer;
    }
}

void GameScene::OnStart ()
{
    for (auto gameObject : gameObjects)
    {
        gameObject->OnStart ();
    }
}

void GameScene::OnFrame ()
{
    timer->Tick ();

    for (auto gameObject : gameObjects)
    {
        gameObject->OnFrame ();
    }

    renderer->RenderFrame ();

    timer->Tock ();
}

void GameScene::OnEnd ()
{
    for (auto gameObject : gameObjects)
    {
        gameObject->OnEnd ();
    }
}

ResourceManager<GameObject>& GameScene::GetGameObjectManager ()
{
    return gameObjects;
}

ResourceManager<Material>& GameScene::GetMaterialManager ()
{
    return materials;
}

Renderer* GameScene::GetRenderer () const
{
    return renderer;
}

Timer* GameScene::GetTimer () const
{
    return timer;
}

void GameScene::SetActiveCamera (const std::string& name)
{
    activeCamera = &(gameObjects.GetByName<Camera> (name));
}

Camera* GameScene::GetActiveCamera () const
{
    return activeCamera;
}

