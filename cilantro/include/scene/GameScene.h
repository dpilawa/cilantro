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
#include <memory>

namespace cilantro {

// This class represents a game world (a.k.a scene or level)
// It contains all visible and invisible objects in a game
class __CEAPI GameScene : public Resource, public std::enable_shared_from_this<GameScene>
{
public:

    __EAPI GameScene (std::shared_ptr<Game> game);
    __EAPI virtual ~GameScene ();

    __EAPI void OnStart ();
    __EAPI void OnFrame ();
    __EAPI void OnEnd ();

    // create a new GameObject in the scene
    // returns reference to that created object
    template <typename T, typename ...Params>
    std::shared_ptr<T> Create (const std::string& name, Params&&... params)
    requires (std::is_base_of_v<GameObject,T>);

    // add a GameObject to the scene
    // returns reference to that added object
    template <typename T>
    std::shared_ptr<T> Add (const std::string& name, std::shared_ptr<T> gameObject)
    requires (std::is_base_of_v<GameObject,T>);

    // create a new material in the scene 
    // returns reference to that created material
    template <typename T, typename ...Params>
    std::shared_ptr<T> Create (const std::string& name, Params&&... params)
    requires (std::is_base_of_v<Material,T>);

    // return reference to map
    __EAPI std::shared_ptr<ResourceManager<GameObject>> GetGameObjectManager ();
    __EAPI std::shared_ptr<ResourceManager<Material>> GetMaterialManager ();

    // renderer control
    template <typename T, typename ...Params> 
    std::shared_ptr<T> Create (Params&&... params)
    requires (std::is_base_of_v<IRenderer,T>);

    __EAPI std::shared_ptr<IRenderer> GetRenderer () const;

    // other getters
    __EAPI std::shared_ptr<Timer> GetTimer () const;
    __EAPI std::shared_ptr<Game> GetGame () const;

    // active camera manipulation
    __EAPI void SetActiveCamera (const std::string& name);
    __EAPI std::shared_ptr<Camera> GetActiveCamera () const;

private:
    
    // game reference
    std::weak_ptr<Game> m_game;

    // map of all GameObjects in the scene
    std::shared_ptr<ResourceManager<GameObject>> m_gameObjectManager;

    // map of all Materials in the scene
    std::shared_ptr<ResourceManager<Material>> m_materialManager;

    // systems
    std::shared_ptr<Timer> m_timer;
    std::shared_ptr<IRenderer> m_renderer;

    // reference to active camera
    std::weak_ptr<Camera> m_activeCamera;

};

template <typename T, typename ...Params>
std::shared_ptr<T> GameScene::Create (const std::string& name, Params&&... params)
    requires (std::is_base_of_v<GameObject,T>)
{
    auto gameObject = m_gameObjectManager->Create<T> (name, shared_from_this (), params...);
    handle_t handle = gameObject->GetHandle ();

    // update renderer data
    if constexpr (std::is_base_of<MeshObject, T>::value)
    {
        GetGame ()->GetMessageBus ()->Publish<MeshObjectUpdateMessage> (std::make_shared<MeshObjectUpdateMessage> (handle));
        GetGame ()->GetMessageBus ()->Publish<SceneGraphUpdateMessage> (std::make_shared<SceneGraphUpdateMessage> (handle));
    }
    else if constexpr (std::is_base_of<Light, T>::value)
    {
        GetGame ()->GetMessageBus ()->Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (handle));
    }

    // return object reference
    return gameObject;
}

template <typename T>
std::shared_ptr<T> GameScene::Add (const std::string& name, std::shared_ptr<T> gameObject)
requires (std::is_base_of_v<GameObject,T>)
{
    m_gameObjectManager->Add<T> (name, gameObject);
    handle_t handle = gameObject->GetHandle ();

    // update renderer data
    if constexpr (std::is_base_of<MeshObject, T>::value)
    {
        GetGame ()->GetMessageBus ()->Publish<MeshObjectUpdateMessage> (std::make_shared<MeshObjectUpdateMessage> (handle));
        GetGame ()->GetMessageBus ()->Publish<SceneGraphUpdateMessage> (std::make_shared<SceneGraphUpdateMessage> (handle));
    }
    else if constexpr (std::is_base_of<Light, T>::value)
    {
        GetGame ()->GetMessageBus ()->Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (handle));
    }

    // return object reference
    return gameObject;
}

template <typename T, typename ...Params>
std::shared_ptr<T> GameScene::Create (const std::string& name, Params&&... params)
    requires (std::is_base_of_v<Material,T>)
{
    auto material = m_materialManager->Create<T> (name, shared_from_this (), params...);
    handle_t handle = material->GetHandle ();

    // update renderer data
    GetGame ()->GetMessageBus ()->Publish<MaterialUpdateMessage> (std::make_shared<MaterialUpdateMessage> (handle));

    // return material reference
    return material;
}

template <typename T, typename ...Params> 
std::shared_ptr<T> GameScene::Create (Params&&... params)
    requires (std::is_base_of_v<IRenderer,T>)
{
    auto newRenderer = std::make_shared<T> (shared_from_this (), params...);
    this->m_renderer = newRenderer;
    this->m_renderer->Initialize ();

    return newRenderer;
}

} // namespace cilantro

#endif

