#ifndef _GAME_H_
#define _GAME_H_

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include "system/MessageBus.h"

class CResource;
class CGameScene;
class InputController;

class CGame
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
    static __EAPI CResourceManager<CGameScene>& GetGameSceneManager ();

    // scene control
    template <typename T, typename ...Params> 
    static T& Create (Params&&... params)
    requires (std::is_base_of_v<CGameScene,T>);
    static __EAPI CGameScene& GetCurrentGameScene ();
    static __EAPI void SetCurrentGameScene (const std::string sceneName);

    // input control
    template <typename T, typename ...Params>
    static T& Create (Params&&... params)
    requires (std::is_base_of_v<InputController,T>);
    static __EAPI InputController& GetInputController ();

    // message bus
    static __EAPI MessageBus& GetMessageBus ();

private:
    
    static __EAPI CResourceManager<CResource> m_resourceManager;
    static __EAPI CResourceManager<CGameScene> m_gameSceneManager;
    static __EAPI CGameScene* m_currentGameScene;
    static __EAPI InputController* m_inputController;
    static __EAPI MessageBus* m_messageBus;

    // game state
    static bool m_shouldStop;
    static bool m_isRunning;

};

template <typename T, typename ...Params> 
T& CGame::Create (Params&&... params)
    requires (std::is_base_of_v<CGameScene,T>)
{
    T& gameScene = m_gameSceneManager.Create<T> (params...);

    if (m_currentGameScene == nullptr)
    {
        m_currentGameScene = &gameScene;
    }

    return gameScene;
}


template <typename T, typename ...Params> 
T& CGame::Create (Params&&... params)
    requires (std::is_base_of_v<InputController,T>)
{
    T* newInputController = new T (params...);

    CGame::m_inputController = static_cast<InputController*> (newInputController);

    return *newInputController;
}

#endif

