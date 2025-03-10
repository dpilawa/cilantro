#include "system/Game.h"
#include "scene/GameScene.h"
#include "input/InputController.h"
#include "system/LogMessage.h"

namespace cilantro {

ResourceManager<Resource> Game::m_resourceManager;
ResourceManager<GameScene> Game::m_gameSceneManager;

GameScene* Game::m_currentGameScene;
InputController* Game::m_inputController;
MessageBus* Game::m_messageBus;

bool Game::m_shouldStop;
bool Game::m_isRunning;
std::string Game::m_path;

void Game::Initialize (std::string path)
{
    LogMessage () << "Engine starting";

    // set path
    m_path = path;

    // set flags
    m_shouldStop = false;
    m_isRunning = false;

    // create message bus
    m_messageBus = new MessageBus ();
}

void Game::Deinitialize ()
{
    if (m_inputController != nullptr)
    {
        m_inputController->Deinitialize ();
        delete m_inputController;
    }
    
    delete m_messageBus;

    LogMessage () << "Engine stopping";
}

ResourceManager<Resource>& Game::GetResourceManager ()
{
    return m_resourceManager;
}

ResourceManager<GameScene>& Game::GetGameSceneManager ()
{
    return m_gameSceneManager;
}

GameScene& Game::GetCurrentGameScene ()
{
    return *m_currentGameScene;
}

void Game::SetCurrentGameScene (const std::string sceneName)
{
    m_currentGameScene = &m_gameSceneManager.GetByName<GameScene>(sceneName);
}

InputController& Game::GetInputController ()
{
    return *m_inputController;
}

MessageBus& Game::GetMessageBus ()
{
    return *m_messageBus;
}

void Game::Run ()
{	
    // initialize all game scenes
    for (auto gameScene : m_gameSceneManager)
    {
        gameScene->OnStart ();
    }

    m_isRunning = true;

    // run game loop, terminate when shouldStop condition is met
    while (m_shouldStop != true) {
        Step ();
    }

    m_isRunning = false;

    // deinitialize all game objects
    for (auto gameScene : m_gameSceneManager)
    {
        gameScene->OnEnd ();
    }
}

void Game::Stop ()
{
    // stop game loop
    m_shouldStop = true;
}

void Game::Step ()
{
    // step current scene
    m_currentGameScene->OnFrame ();

    // process input
    m_inputController->OnFrame ();
}

std::string Game::GetPath ()
{
    return m_path;
}

bool Game::IsRunning ()
{
    return m_isRunning;
}

} // namespace cilantro