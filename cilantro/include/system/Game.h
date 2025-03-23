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

class __CEAPI Game : public std::enable_shared_from_this<Game>
{
public:
    __EAPI Game ();
    __EAPI virtual ~Game ();

    // initialize and deinitialize
    __EAPI void Initialize ();
    __EAPI void Deinitialize ();

    // run a game loop
    __EAPI void Run ();
    __EAPI void Stop ();	
    __EAPI void Step ();

    // get global parameters
    __EAPI bool IsRunning ();

    // managers
    __EAPI ResourceManager<Resource>& GetResourceManager ();
    __EAPI ResourceManager<GameScene>& GetGameSceneManager ();

    // scene control
    template <typename T, typename ...Params> 
    std::shared_ptr<T> Create (const std::string& name, Params&&... params)
    requires (std::is_base_of_v<GameScene,T>);
    
    __EAPI std::shared_ptr<GameScene> GetCurrentGameScene ();
    __EAPI void SetCurrentGameScene (const std::string sceneName);

    // input control
    template <typename T, typename ...Params>
    std::shared_ptr<T> Create (Params&&... params)
    requires (std::is_base_of_v<InputController,T>);
    
    __EAPI std::shared_ptr<InputController> GetInputController ();

    // message bus
    __EAPI std::shared_ptr<MessageBus> GetMessageBus ();

private:
    
    ResourceManager<Resource> m_resourceManager;
    ResourceManager<GameScene> m_gameSceneManager;
    std::shared_ptr<GameScene> m_currentGameScene;
    std::shared_ptr<InputController> m_inputController;
    std::shared_ptr<MessageBus> m_messageBus;

    // game state
    bool m_shouldStop;
    bool m_isRunning;

};

template <typename T, typename ...Params> 
std::shared_ptr<T> Game::Create (const std::string& name, Params&&... params)
    requires (std::is_base_of_v<GameScene,T>)
{
    auto gameScene = m_gameSceneManager.Create<T> (name, shared_from_this (), std::forward<Params>(params)...);

    if (m_currentGameScene == nullptr)
    {
        m_currentGameScene = gameScene;
    }

    return gameScene;
}


template <typename T, typename ...Params> 
std::shared_ptr<T> Game::Create (Params&&... params)
    requires (std::is_base_of_v<InputController,T>)
{
    m_inputController = std::make_shared<T> (shared_from_this (), std::forward<Params>(params)...);

    return std::static_pointer_cast<T> (m_inputController);
}

} // namespace cilantro

#endif

