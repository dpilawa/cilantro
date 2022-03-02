#ifndef _GAMESCENE_H_
#define _GAMESCENE_H_

#include "cilantroengine.h"
#include "resource/Resource.h"
#include "resource/ResourceManager.h"
#include "graphics/Renderer.h"
#include "input/InputController.h"
#include "system/Timer.h"
#include "scene/GameObject.h"
#include "scene/MeshObject.h"
#include "scene/Material.h"
#include "scene/Camera.h"
#include "scene/Light.h"
#include <string>

// This class represents a game world (a.k.a scene or level)
// It contains all visible and invisible objects in a game
class GameScene : public Resource, public CallbackProvider<std::string, handle_t, unsigned int>
{
public:

    __EAPI GameScene();
    __EAPI ~GameScene();

    void OnStart ();
    void OnFrame ();
    void OnEnd ();

    // add GameObject to a scene
    // returns reference to that added object
    template <typename T, typename ...Params>
    T& AddGameObject (const std::string& name, Params&&... params);

    // add material to the scene 
    // returns reference to that material
    template <typename T, typename ...Params>
    T& AddMaterial (const std::string& name, Params&&... params);

    // return reference to map
    __EAPI ResourceManager<GameObject>& GetGameObjectManager ();
    __EAPI ResourceManager<Material>& GetMaterialManager ();

    // renderer control
    template <typename T, typename ...Params> 
    T& CreateRenderer (Params&&... params);

    __EAPI Renderer* GetRenderer () const;

    // other getters
    __EAPI Timer* GetTimer () const;

    // active camera manipulation
    __EAPI void SetActiveCamera (const std::string& name);
    __EAPI Camera* GetActiveCamera () const;

private:
    
    // map of all GameObjects in the scene
    ResourceManager<GameObject> gameObjects;

    // map of all Materials in the scene
    ResourceManager<Material> materials;

    // systems
    Timer* timer;
    Renderer* renderer;

    // reference to active camera
    Camera* activeCamera;

};

template <typename T, typename ...Params>
T& GameScene::AddGameObject (const std::string& name, Params&&... params)
{
    T& gameObject = gameObjects.Create<T> (name, this, params...);
    handle_t handle = gameObject.GetHandle ();

    // set callbacks on object modification
    // this is just a passthrough of callbacks to subscribers (e.g. Renderer)
    gameObject.RegisterCallback ("OnUpdateMeshObject", [ & ](unsigned int objectHandle, unsigned int) { InvokeCallbacks ("OnUpdateMeshObject", objectHandle, 0); });
    gameObject.RegisterCallback ("OnUpdateLight", [ & ](unsigned int objectHandle, unsigned int) { InvokeCallbacks ("OnUpdateLight", objectHandle, 0); });
    gameObject.RegisterCallback ("OnUpdateSceneGraph", [ & ](unsigned int objectHandle, unsigned int) { InvokeCallbacks ("OnUpdateSceneGraph", objectHandle, 0); });
    gameObject.RegisterCallback ("OnUpdateTransform", [&](unsigned int objectHandle, unsigned int) { 
        InvokeCallbacks ("OnUpdateTransform", objectHandle, 0);
    });

    // update renderer data
    if constexpr (std::is_base_of<MeshObject, T>::value)
    {
        InvokeCallbacks ("OnUpdateMeshObject", handle, 0);
        InvokeCallbacks ("OnUpdateSceneGraph", handle, 0);
    }
    else if constexpr (std::is_base_of<Light, T>::value)
    {
        InvokeCallbacks ("OnUpdateLight", handle, 0);
    }

    // return object reference
    return gameObject;
}

template <typename T, typename ...Params>
T& GameScene::AddMaterial (const std::string& name, Params&&... params)
{
    T& material = materials.Create<T> (name, params...);
    handle_t handle = material.GetHandle ();

    // register callbacks
    material.RegisterCallback ("OnUpdateMaterialTexture", [ & ](unsigned int materialHandle, unsigned int textureUnit) { InvokeCallbacks ("OnUpdateMaterialTexture", materialHandle, textureUnit); });
    material.RegisterCallback ("OnUpdateMaterial", [ & ](unsigned int materialHandle, unsigned int) { InvokeCallbacks ("OnUpdateMaterial", materialHandle, 0); });

    // update renderer data
    InvokeCallbacks("OnUpdateMaterial", handle, 0);

    // return material reference
    return material;
}

template <typename T, typename ...Params> 
T& GameScene::CreateRenderer (Params&&... params)
{
    static_assert (std::is_base_of<Renderer, T>::value, "Renderer object must inherit from Renderer");
    T* newRenderer = new T (this, params...);

    this->renderer = static_cast<Renderer*> (newRenderer);
    this->renderer->Initialize ();

    return *newRenderer;
}

#endif

