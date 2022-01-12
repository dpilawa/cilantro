#include "cilantroengine.h"
#include "system/Game.h"
#include "system/LogMessage.h"

ResourceManager<Resource>* Game::contextResourceManager;
Timer* Game::contextTimer;
GameScene* Game::contextGameScene;
Renderer* Game::contextRenderer;
InputController* Game::contextInputController;

bool Game::shouldStop;
bool Game::isRunning;

void Game::Initialize (ResourceManager<Resource>& resourceManager, Timer& timer, GameScene& gameScene, Renderer& renderer, InputController& inputController)
{
    LogMessage () << "Engine starting";

    // set flags
    shouldStop = false;
    isRunning = false;

    contextResourceManager = &resourceManager;
    contextTimer = &timer;
    contextGameScene = &gameScene;
    contextRenderer = &renderer;
    contextInputController = &inputController;

    contextRenderer->Initialize ();
    contextInputController->Initialize ();

    // pre-set game clocks
    contextTimer->Tick ();
}

void Game::Deinitialize ()
{
    contextInputController->Deinitialize ();
    contextRenderer->Deinitialize ();

    LogMessage () << "Engine stopped";
}

ResourceManager<Resource>& Game::GetResourceManager ()
{
    return *contextResourceManager;
}

Timer& Game::GetTimer ()
{
    return *contextTimer;
}

GameScene& Game::GetGameScene ()
{
    return *contextGameScene;
}

Renderer& Game::GetRenderer ()
{
    return *contextRenderer;
}

InputController& Game::GetInputController ()
{
    return *contextInputController;
}

void Game::Run ()
{	
    // initialize all game objects
    for (auto gameObject : contextGameScene->GetGameObjectManager ())
    {
        gameObject->OnStart ();
    }

    isRunning = true;

    // run game loop, terminate when shouldStop condition is met
    while (shouldStop != true) {
        Step ();
    }

    isRunning = false;

    // deinitialize all game objects
    for (auto gameObject : contextGameScene->GetGameObjectManager ())
    {
        gameObject->OnEnd ();
    }
}

void Game::Stop ()
{
    // stop game loop
    shouldStop = true;
}

void Game::Step ()
{
    // update game clocks (Tick)
    contextTimer->Tick ();

    // process all game objects
    for (auto gameObject : contextGameScene->GetGameObjectManager ())
    {
        gameObject->OnFrame ();
    }

    // render frame
    contextRenderer->RenderFrame ();

    // process controller events
    contextInputController->OnFrame ();

}



