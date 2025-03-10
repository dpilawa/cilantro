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

CGameScene::CGameScene()
{ 
    this->timer = new Timer();
    timer->Tick ();

    this->activeCamera = nullptr;
}

CGameScene::~CGameScene()
{
    delete timer;

    if (renderer != nullptr)
    {
        renderer->Deinitialize ();
        delete renderer;
    }
}

void CGameScene::OnStart ()
{
    for (auto gameObject : gameObjects)
    {
        gameObject->OnStart ();
    }
}

void CGameScene::OnFrame ()
{
    timer->Tick ();

    for (auto gameObject : gameObjects)
    {
        gameObject->OnFrame ();
    }

    renderer->RenderFrame ();

    timer->Tock ();
}

void CGameScene::OnEnd ()
{
    for (auto gameObject : gameObjects)
    {
        gameObject->OnEnd ();
    }
}

CResourceManager<GameObject>& CGameScene::GetGameObjectManager ()
{
    return gameObjects;
}

CResourceManager<Material>& CGameScene::GetMaterialManager ()
{
    return materials;
}

IRenderer* CGameScene::GetRenderer () const
{
    return renderer;
}

Timer* CGameScene::GetTimer () const
{
    return timer;
}

void CGameScene::SetActiveCamera (const std::string& name)
{
    activeCamera = &(gameObjects.GetByName<Camera> (name));
}

Camera* CGameScene::GetActiveCamera () const
{
    if (activeCamera == nullptr)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "No active camera found";
    }

    return activeCamera;
}

} // namespace cilantro

