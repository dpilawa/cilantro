#include "system/Game.h"
#include "scene/GameScene.h"
#include "input/InputController.h"
#include "system/LogMessage.h"

namespace cilantro {

Game::Game ()
{
    m_inputController = nullptr;
    m_messageBus = nullptr;

    m_resourceManager = std::make_shared<ResourceManager<Resource>> ();
    m_gameSceneManager = std::make_shared<ResourceManager<GameScene>> ();
}

Game::~Game ()
{
}

void Game::Initialize ()
{
    LogMessage () << "Engine starting";

    // set flags
    m_shouldStop = false;
    m_isRunning = false;

    // create message bus
    m_messageBus = std::make_shared<MessageBus> ();
}

void Game::Deinitialize ()
{
    if (m_inputController != nullptr)
    {
        m_inputController->Deinitialize ();
    }  

    LogMessage () << "Engine stopping";
}

std::shared_ptr<ResourceManager<Resource>> Game::GetResourceManager ()
{
    return m_resourceManager;
}

std::shared_ptr<ResourceManager<GameScene>> Game::GetGameSceneManager ()
{
    return m_gameSceneManager;
}

std::shared_ptr<GameScene> Game::GetCurrentGameScene ()
{
    return m_currentGameScene.lock ();
}

void Game::SetCurrentGameScene (const std::string sceneName)
{
    m_currentGameScene = m_gameSceneManager->GetByName<GameScene>(sceneName);
}

std::shared_ptr<InputController> Game::GetInputController ()
{
    return m_inputController;
}

std::shared_ptr<MessageBus> Game::GetMessageBus ()
{
    return m_messageBus;
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
    m_currentGameScene.lock ()->OnFrame ();

    // process input
    m_inputController->OnFrame ();
}

bool Game::IsRunning ()
{
    return m_isRunning;
}

} // namespace cilantro