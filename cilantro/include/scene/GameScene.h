#ifndef _GAMESCENE_H_
#define _GAMESCENE_H_

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include "game/GameComposite.h"
#include "scene/GameObject.h"
#include "scene/Material.h"
#include "scene/Camera.h"
#include <string>

// This class represents a game world (a.k.a scene or level)
// It contains all visible and invisible objects in a game
class GameScene : public GameComposite, public CallbackProvider<std::string, unsigned int, unsigned int>
{
public:

    __EAPI GameScene();
    __EAPI ~GameScene();

    // add GameObject to a scene
    // returns reference to that added object
    __EAPI GameObject& AddGameObject (GameObject* gameObject);

    // get game object by its handle
    __EAPI GameObject& GetObjectByHandle (unsigned int handle);

    // add material to the scene 
    // returns reference to that material
    template <typename T>
    T& AddMaterial (const std::string& name);

    // return reference to map
    __EAPI std::unordered_map <unsigned int, GameObject*>& GetGameObjects ();
    __EAPI ResourceManager<Material>& GetMaterialManager ();

    // return number of scene's GameObjects
    __EAPI unsigned int getGameObjectsCount () const;

    // active camera manipulation
    __EAPI void SetActiveCamera (Camera* camera);
    __EAPI Camera* GetActiveCamera () const;

private:
    
    // map of all GameObjects in the scene
    std::unordered_map <unsigned int, GameObject*> gameObjects;

    // map of all Materials in the scene
    ResourceManager<Material> materials;

    // counts of objects in the scene
    unsigned int gameObjectsCount;

    // reference to active camera
    Camera* activeCamera;

};

template <typename T>
T& GameScene::AddMaterial (const std::string& name)
{
    T& material = materials.Create<T> (name);

    // set game loop reference
    material.AttachToGame (this->game);

    // register callbacks
    material.RegisterCallback ("OnUpdateMaterial", [ & ](unsigned int materialHandle, unsigned int textureUnit) { InvokeCallbacks ("OnUpdateMaterial", materialHandle, textureUnit); });

    // return material handle
    return material;
}

#endif

