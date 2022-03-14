#ifndef _GAME_H_
#define _GAME_H_

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include "input/InputController.h"

class CResource;
class GameScene;
class InputController;

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
    static __EAPI CResourceManager<CResource>& GetResourceManager ();
    static __EAPI CResourceManager<GameScene>& GetGameSceneManager ();

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
    
    static __EAPI CResourceManager<CResource> m_resourceManager;
    static __EAPI CResourceManager<GameScene> m_gameSceneManager;
    static __EAPI GameScene* m_currentGameScene;
    static __EAPI InputController* inputController;

    // game state
    static bool m_shouldStop;
    static bool m_isRunning;

};

template <typename T, typename ...Params> 
T& Game::CreateGameScene (Params&&... params)
{
    static_assert (std::is_base_of<GameScene, T>::value, "Game scene object must inherit from GameScene");
    T& gameScene = m_gameSceneManager.Create<T> (params...);

    if (m_currentGameScene == nullptr)
    {
        m_currentGameScene = &gameScene;
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

