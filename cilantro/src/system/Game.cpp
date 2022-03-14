#include "system/Game.h"
#include "scene/GameScene.h"
#include "input/InputController.h"
#include "system/LogMessage.h"

CResourceManager<CResource> Game::m_resourceManager;
CResourceManager<GameScene> Game::m_gameSceneManager;

GameScene* Game::m_currentGameScene;
InputController* Game::inputController;

bool Game::m_shouldStop;
bool Game::m_isRunning;

void Game::Initialize ()
{
    LogMessage () << "Engine starting";

    // set flags
    m_shouldStop = false;
    m_isRunning = false;
}

void Game::Deinitialize ()
{
    if (inputController != nullptr)
    {
        inputController->Deinitialize ();
        delete inputController;
    }
    
    LogMessage () << "Engine stopping";
}

CResourceManager<CResource>& Game::GetResourceManager ()
{
    return m_resourceManager;
}

CResourceManager<GameScene>& Game::GetGameSceneManager ()
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
    return *inputController;
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
    inputController->OnFrame ();
}



