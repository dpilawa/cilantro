#include "system/Game.h"
#include "scene/GameScene.h"
#include "input/InputController.h"
#include "system/LogMessage.h"

ResourceManager<Resource> globalResourceManager;
ResourceManager<GameScene> globalSceneManager;

ResourceManager<Resource> Game::resourceManager;
ResourceManager<GameScene> Game::gameSceneManager;

GameScene* Game::currentGameScene;
InputController* Game::inputController;

bool Game::shouldStop;
bool Game::isRunning;

void Game::Initialize ()
{
    LogMessage () << "Engine starting";

    // set flags
    shouldStop = false;
    isRunning = false;
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

ResourceManager<Resource>& Game::GetResourceManager ()
{
    return resourceManager;
}

ResourceManager<GameScene>& Game::GetGameSceneManager ()
{
    return gameSceneManager;
}

GameScene& Game::GetCurrentGameScene ()
{
    return *currentGameScene;
}

void Game::SetCurrentGameScene (const std::string sceneName)
{
    currentGameScene = &gameSceneManager.GetByName<GameScene>(sceneName);
}

InputController& Game::GetInputController ()
{
    return *inputController;
}

void Game::Run ()
{	
    // initialize all game scenes
    for (auto gameScene : gameSceneManager)
    {
        gameScene->OnStart ();
    }

    isRunning = true;

    // run game loop, terminate when shouldStop condition is met
    while (shouldStop != true) {
        Step ();
    }

    isRunning = false;

    // deinitialize all game objects
    for (auto gameScene : gameSceneManager)
    {
        gameScene->OnEnd ();
    }
}

void Game::Stop ()
{
    // stop game loop
    shouldStop = true;
}

void Game::Step ()
{
    // step current scene
    currentGameScene->OnFrame ();

    // process input
    inputController->OnFrame ();
}



