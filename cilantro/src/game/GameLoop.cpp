#include "cilantroengine.h"
#include "game/GameLoop.h"
#include "scene/GameScene.h"
#include "graphics/Renderer.h"
#include "input/InputController.h"
#include "util/LogMessage.h"
#include "util/Timer.h"

GameLoop::GameLoop ()
{
    LogMessage (__func__) << "Engine starting";

    gameScene = nullptr;
    gameRenderer = nullptr;
    gameInputController = nullptr;
    gameRenderTarget = nullptr;

    // set flags
    shouldStop = false;

    // pre-set game clocks
    Timer::Tick ();
}

GameLoop::~GameLoop ()
{
    LogMessage (__func__) << "Engine stopped";
}

void GameLoop::Run ()
{	
    // initialize all game objects
    for (auto gameObject : gameScene->GetGameObjects ())
    {
        gameObject.second->OnStart ();
    }

    // run game loop, terminate when shouldStop condition is met
    while (shouldStop != true) {
        Step ();
    }

    // deinitialize all game objects
    for (auto gameObject : gameScene->GetGameObjects ())
    {
        gameObject.second->OnEnd ();
    }
}

void GameLoop::Stop ()
{
    // stop game loop
    shouldStop = true;
}

void GameLoop::Step ()
{
    // update game clocks (Tick)
    Timer::Tick ();

    // process all game objects
    for (auto gameObject : gameScene->GetGameObjects ())
    {
        gameObject.second->OnFrame ();
    }

    // render frame
    gameRenderer->RenderFrame ();

    // process rendertarget
    gameRenderTarget->OnFrame ();

    // process controller events
    gameInputController->OnFrame ();

}



