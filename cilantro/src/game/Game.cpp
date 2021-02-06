#include "cilantroengine.h"
#include "game/Game.h"
#include "resource/ResourceManager.h"
#include "scene/GameScene.h"
#include "graphics/Renderer.h"
#include "input/InputController.h"
#include "system/LogMessage.h"
#include "system/Timer.h"

Game::Game (ResourceManager<Resource>& resourceManager, GameScene& gameScene, Renderer& renderer, RenderTarget& renderTarget, InputController& inputController) 
: resourceManager (resourceManager), gameScene (gameScene), renderer (renderer), renderTarget (renderTarget), inputController (inputController)
{
    LogMessage (MSG_LOCATION) << "Engine starting";

    // set composite references
    gameScene.AttachToGame (this);
    renderTarget.AttachToGame (this);
    renderer.AttachToGame (this);
    inputController.AttachToGame (this);

    // initialize
    renderTarget.Initialize ();
    renderer.Initialize ();
    inputController.Initialize ();

    // set flags
    shouldStop = false;
    isRunning = false;

    // pre-set game clocks
    Timer::Tick ();
}

Game::~Game ()
{
    inputController.Deinitialize ();
    renderer.Deinitialize ();
    renderTarget.Deinitialize ();
 
    LogMessage (MSG_LOCATION) << "Engine stopped";
}

void Game::Run ()
{	
    // initialize all game objects
    for (auto gameObject : gameScene.GetGameObjects ())
    {
        gameObject.second->OnStart ();
    }

    isRunning = true;

    // run game loop, terminate when shouldStop condition is met
    while (shouldStop != true) {
        Step ();
    }

    isRunning = false;

    // deinitialize all game objects
    for (auto gameObject : gameScene.GetGameObjects ())
    {
        gameObject.second->OnEnd ();
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
    Timer::Tick ();

    // process all game objects
    for (auto gameObject : gameScene.GetGameObjects ())
    {
        gameObject.second->OnFrame ();
    }

    // render frame
    renderer.RenderFrame ();

    // process rendertarget
    renderTarget.OnFrame ();

    // process controller events
    inputController.OnFrame ();

}

ResourceManager<Resource>& Game::GetResourceManager ()
{
    return this->resourceManager;
}

GameScene& Game::GetGameScene ()
{
    return this->gameScene;
}

Renderer& Game::GetRenderer ()
{
    return this->renderer;
}

RenderTarget& Game::GetRenderTarget ()
{
    return this->renderTarget;
}

InputController& Game::GetInputController ()
{
    return this->inputController;
}

