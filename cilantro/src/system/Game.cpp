#include "system/Game.h"
#include "scene/GameScene.h"
#include "input/InputController.h"
#include "system/LogMessage.h"

CResourceManager<CResource> CGame::m_resourceManager;
CResourceManager<CGameScene> CGame::m_gameSceneManager;

CGameScene* CGame::m_currentGameScene;
InputController* CGame::m_inputController;
MessageBus* CGame::m_messageBus;

bool CGame::m_shouldStop;
bool CGame::m_isRunning;

void CGame::Initialize ()
{
    LogMessage () << "Engine starting";

    // set flags
    m_shouldStop = false;
    m_isRunning = false;

    // create message bus
    m_messageBus = new MessageBus ();
}

void CGame::Deinitialize ()
{
    if (m_inputController != nullptr)
    {
        m_inputController->Deinitialize ();
        delete m_inputController;
    }
    
    delete m_messageBus;

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
    return *m_inputController;
}

MessageBus& CGame::GetMessageBus ()
{
    return *m_messageBus;
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
    m_inputController->OnFrame ();
}



