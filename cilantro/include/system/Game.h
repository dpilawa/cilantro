#ifndef _GAME_H_
#define _GAME_H_

#include "cilantroengine.h"
#include "resource/Resource.h"
#include "resource/ResourceManager.h"
#include "scene/GameScene.h"
#include "input/InputController.h"

class Game
{
public:

    // initialize and deinitialize
    static __EAPI void Initialize ();
    static __EAPI void Deinitialize ();

    // run a game loop
    static __EAPI void Run ();
    static __EAPI void Stop ();	
    static __EAPI void Step ();

    // managers
    static __EAPI ResourceManager<Resource>& GetResourceManager ();
    static __EAPI ResourceManager<GameScene>& GetGameSceneManager ();

    // scene control
    template <typename T, typename ...Params> 
    static T& CreateGameScene (Params&&... params);
    static __EAPI GameScene& GetCurrentGameScene ();
    static __EAPI void SetCurrentGameScene (const std::string sceneName);

    // input control
    template <typename T, typename ...Params>
    static T& CreateInputController (Params&&... params);
    static __EAPI InputController& GetInputController ();

private:
    
    static __EAPI ResourceManager<Resource>& resourceManager;
    static __EAPI ResourceManager<GameScene>& gameSceneManager;
    static __EAPI GameScene* currentGameScene;
    static __EAPI InputController* inputController;

    // game state
    static bool shouldStop;
    static bool isRunning;

};

template <typename T, typename ...Params> 
T& Game::CreateGameScene (Params&&... params)
{
    static_assert (std::is_base_of<GameScene, T>::value, "Game scene object must inherit from GameScene");
    T& gameScene = gameSceneManager.Create<T> (params...);

    if (currentGameScene == nullptr)
    {
        currentGameScene = &gameScene;
    }

    return gameScene;
}


template <typename T, typename ...Params> 
T& Game::CreateInputController (Params&&... params)
{
    static_assert (std::is_base_of<InputController, T>::value, "Input controller object must inherit from InputController");
    T* newInputController = new T (params...);

    Game::inputController = static_cast<InputController*> (newInputController);
    Game::inputController->Initialize ();

    return *newInputController;
}

#endif

