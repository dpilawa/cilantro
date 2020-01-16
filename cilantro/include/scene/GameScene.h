#ifndef _GAMESCENE_H_
#define _GAMESCENE_H_

#include "cilantroengine.h"
#include "game/GameLoop.h"
#include "scene/GameObject.h"
#include "scene/Material.h"
#include "scene/Camera.h"
#include <string>

// This class represents a game world (a.k.a scene or level)
// It contains all visible and invisible objects in a game
class GameScene : public CallbackProvider<std::string, unsigned int>
{
public:
    GameScene() = delete;
    __EAPI GameScene(GameLoop* gameLoop);
    __EAPI ~GameScene();

    // add GameObject to a scene
    // returns reference to that added object
    __EAPI GameObject& AddGameObject (GameObject* gameObject);

    // get game object by its handle
    __EAPI GameObject& GetObjectByHandle (unsigned int handle);

    // add material to the scene 
    // returns reference to that material
    __EAPI Material& AddMaterial (Material* material);

    // return reference to GameObject vector
    __EAPI std::unordered_map <unsigned int, GameObject*>& GetGameObjects ();

    // return number of scene's GameObjects
    __EAPI unsigned int getGameObjectsCount () const;

    // active camera manipulation
    __EAPI void SetActiveCamera (Camera* camera);
    __EAPI Camera* GetActiveCamera () const;

private:
    
    GameLoop* gameLoop;

    // vector of all GameObjects in the scene
    std::unordered_map <unsigned int, GameObject*> gameObjects;

    // vector of all Materials in the scene
    std::vector <Material*> materials;

    // count of all GameObjects in the scene
    unsigned int gameObjectsCount;

    // reference to active camera
    Camera* activeCamera;

};

#endif

