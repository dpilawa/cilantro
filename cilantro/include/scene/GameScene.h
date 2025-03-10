#ifndef _GAMESCENE_H_
#define _GAMESCENE_H_

#include "cilantroengine.h"
#include "resource/Resource.h"
#include "resource/ResourceManager.h"
#include "graphics/Renderer.h"
#include "input/InputController.h"
#include "system/Timer.h"
#include "system/MessageBus.h"
#include "system/Message.h"
#include "system/Game.h"
#include "scene/GameObject.h"
#include "scene/MeshObject.h"
#include "scene/Material.h"
#include "scene/Camera.h"
#include "scene/Light.h"
#include <string>

namespace cilantro {

// This class represents a game world (a.k.a scene or level)
// It contains all visible and invisible objects in a game
class CGameScene : public CResource
{
public:

    __EAPI CGameScene();
    __EAPI ~CGameScene();

    void OnStart ();
    void OnFrame ();
    void OnEnd ();

    // add GameObject to a scene
    // returns reference to that added object
    template <typename T, typename ...Params>
    T& Add (const std::string& name, Params&&... params)
    requires (std::is_base_of_v<GameObject,T>);

    // add material to the scene 
    // returns reference to that material
    template <typename T, typename ...Params>
    T& Add (const std::string& name, Params&&... params)
    requires (std::is_base_of_v<Material,T>);

    // return reference to map
    __EAPI CResourceManager<GameObject>& GetGameObjectManager ();
    __EAPI CResourceManager<Material>& GetMaterialManager ();

    // renderer control
    template <typename T, typename ...Params> 
    T& Create (Params&&... params)
    requires (std::is_base_of_v<IRenderer,T>);

    __EAPI IRenderer* GetRenderer () const;

    // other getters
    __EAPI Timer* GetTimer () const;

    // active camera manipulation
    __EAPI void SetActiveCamera (const std::string& name);
    __EAPI Camera* GetActiveCamera () const;

private:
    
    // map of all GameObjects in the scene
    CResourceManager<GameObject> gameObjects;

    // map of all Materials in the scene
    CResourceManager<Material> materials;

    // systems
    Timer* timer;
    IRenderer* renderer;

    // reference to active camera
    Camera* activeCamera;

};

template <typename T, typename ...Params>
T& CGameScene::Add (const std::string& name, Params&&... params)
    requires (std::is_base_of_v<GameObject,T>)
{
    T& gameObject = gameObjects.Create<T> (name, this, params...);
    handle_t handle = gameObject.GetHandle ();

    // update renderer data
    if constexpr (std::is_base_of<MeshObject, T>::value)
    {
        CGame::GetMessageBus ().Publish<MeshObjectUpdateMessage> (std::make_shared<MeshObjectUpdateMessage> (handle));
        CGame::GetMessageBus ().Publish<SceneGraphUpdateMessage> (std::make_shared<SceneGraphUpdateMessage> (handle));
    }
    else if constexpr (std::is_base_of<Light, T>::value)
    {
        CGame::GetMessageBus ().Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (handle));
    }

    // return object reference
    return gameObject;
}

template <typename T, typename ...Params>
T& CGameScene::Add (const std::string& name, Params&&... params)
    requires (std::is_base_of_v<Material,T>)
{
    T& material = materials.Create<T> (name, params...);
    handle_t handle = material.GetHandle ();

    // update renderer data
    CGame::GetMessageBus ().Publish<MaterialUpdateMessage> (std::make_shared<MaterialUpdateMessage> (handle));

    // return material reference
    return material;
}

template <typename T, typename ...Params> 
T& CGameScene::Create (Params&&... params)
    requires (std::is_base_of_v<IRenderer,T>)
{
    T* newRenderer = new T (this, params...);

    this->renderer = static_cast<IRenderer*> (newRenderer);
    this->renderer->Initialize ();

    return *newRenderer;
}

} // namespace cilantro

#endif

