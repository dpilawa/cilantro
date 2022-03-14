#include "system/Game.h"
#include "scene/GameScene.h"
#include "input/InputController.h"
#include "system/LogMessage.h"

CResourceManager<CResource> CGame::m_resourceManager;
CResourceManager<CGameScene> CGame::m_gameSceneManager;

CGameScene* CGame::m_currentGameScene;
InputController* CGame::inputController;

bool CGame::m_shouldStop;
bool CGame::m_isRunning;

void CGame::Initialize ()
{
    LogMessage () << "Engine starting";

    // set flags
    m_shouldStop = false;
    m_isRunning = false;
}

void CGame::Deinitialize ()
{
    if (inputController != nullptr)
    {
        inputController->Deinitialize ();
        delete inputController;
    }
    
    LogMessage () << "Engine stopping";
}

CResourceManager<CResource>& CGame::GetResourceManager ()
{
    return m_resourceManager;
}

CResourceManager<CGameScene>& CGame::GetGameSceneManager ()
{
    return m_gameSceneManager;
}

CGameScene& CGame::GetCurrentGameScene ()
{
    return *m_currentGameScene;
}

void CGame::SetCurrentGameScene (const std::string sceneName)
{
    m_currentGameScene = &m_gameSceneManager.GetByName<CGameScene>(sceneName);
}

InputController& CGame::GetInputController ()
{
    return *inputController;
}

void CGame::Run ()
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

void CGame::Stop ()
{
    // stop game loop
    m_shouldStop = true;
}

void CGame::Step ()
{
    // step current scene
    m_currentGameScene->OnFrame ();

    // process input
    inputController->OnFrame ();
}



