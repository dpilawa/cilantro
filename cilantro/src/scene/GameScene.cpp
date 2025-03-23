#include "cilantroengine.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/Camera.h"
#include "scene/Material.h"
#include "scene/PBRMaterial.h"
#include "scene/PhongMaterial.h"
#include "system/LogMessage.h"

#include <vector>

namespace cilantro {

GameScene::GameScene(std::shared_ptr<Game> game)
{ 
    this->m_game = game;
    this->m_timer = std::make_shared<Timer> ();
    m_timer->Tick ();

    this->m_renderer = nullptr;
}

GameScene::~GameScene()
{
    if (m_renderer != nullptr)
    {
        m_renderer->Deinitialize ();
    }
}

void GameScene::OnStart ()
{
    for (auto gameObject : m_gameObjects)
    {
        gameObject->OnStart ();
    }
}

void GameScene::OnFrame ()
{
    m_timer->Tick ();

    for (auto gameObject : m_gameObjects)
    {
        gameObject->OnFrame ();
    }

    m_renderer->RenderFrame ();

    m_timer->Tock ();
}

void GameScene::OnEnd ()
{
    for (auto gameObject : m_gameObjects)
    {
        gameObject->OnEnd ();
    }
}

ResourceManager<GameObject>& GameScene::GetGameObjectManager ()
{
    return m_gameObjects;
}

ResourceManager<Material>& GameScene::GetMaterialManager ()
{
    return m_materials;
}

std::shared_ptr<IRenderer> GameScene::GetRenderer () const
{
    return m_renderer;
}

std::shared_ptr<Timer> GameScene::GetTimer () const
{
    return m_timer;
}

std::shared_ptr<Game> GameScene::GetGame () const
{
    return m_game.lock ();
}

void GameScene::SetActiveCamera (const std::string& name)
{
    m_activeCamera = m_gameObjects.GetByName<Camera> (name);
}

std::shared_ptr<Camera> GameScene::GetActiveCamera () const
{
    auto camera = m_activeCamera.lock ();
    if (camera == nullptr)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "No active camera found";
    }

    return camera;
}

} // namespace cilantro

