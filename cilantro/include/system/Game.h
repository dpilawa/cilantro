#ifndef _GAME_H_
#define _GAME_H_

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include "system/MessageBus.h"
#include <string>

namespace cilantro {

class Resource;
class GameScene;
class InputController;

class __CEAPI Game
{
public:

    // initialize and deinitialize
    static __EAPI void Initialize (std::string path);
    static __EAPI void Deinitialize ();

    // run a game loop
    static __EAPI void Run ();
    static __EAPI void Stop ();	
    static __EAPI void Step ();

    // get global parameters
    static __EAPI std::string GetPath ();
    static __EAPI bool IsRunning ();

    // managers
    static __EAPI ResourceManager<Resource>& GetResourceManager ();
    static __EAPI ResourceManager<GameScene>& GetGameSceneManager ();

    // scene control
    template <typename T, typename ...Params> 
    static T& Create (Params&&... params)
    requires (std::is_base_of_v<GameScene,T>);
    
    static __EAPI GameScene& GetCurrentGameScene ();
    static __EAPI void SetCurrentGameScene (const std::string sceneName);

    // input control
    template <typename T, typename ...Params>
    static T& Create (Params&&... params)
    requires (std::is_base_of_v<InputController,T>);
    static __EAPI InputController& GetInputController ();

    // message bus
    static __EAPI MessageBus& GetMessageBus ();

private:
    
    static __EAPI ResourceManager<Resource> m_resourceManager;
    static __EAPI ResourceManager<GameScene> m_gameSceneManager;
    static __EAPI GameScene* m_currentGameScene;
    static __EAPI InputController* m_inputController;
    static __EAPI MessageBus* m_messageBus;

    // game state
    static std::string m_path;
    static bool m_shouldStop;
    static bool m_isRunning;

};

template <typename T, typename ...Params> 
T& Game::Create (Params&&... params)
    requires (std::is_base_of_v<GameScene,T>)
{
    T& gameScene = m_gameSceneManager.Create<T> (params...);

    if (m_currentGameScene == nullptr)
    {
        m_currentGameScene = &gameScene;
    }

    return gameScene;
}


template <typename T, typename ...Params> 
T& Game::Create (Params&&... params)
    requires (std::is_base_of_v<InputController,T>)
{
    T* newInputController = new T (params...);

    Game::m_inputController = static_cast<InputController*> (newInputController);

    return *newInputController;
}

} // namespace cilantro

#endif

