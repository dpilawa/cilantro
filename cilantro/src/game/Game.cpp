#include "cilantroengine.h"
#include "game/Game.h"
#include "system/EngineContext.h"
#include "system/LogMessage.h"

Game::Game ()
{
    LogMessage (MSG_LOCATION) << "Engine starting";

    // set flags
    shouldStop = false;
    isRunning = false;
}

Game::~Game ()
{
    LogMessage (MSG_LOCATION) << "Engine stopped";
}

void Game::Initialize ()
{
    // pre-set game clocks
    EngineContext::GetTimer ().Tick ();
}

void Game::Deinitialize ()
{
    
}

void Game::Run ()
{	
    // initialize all game objects
    for (auto gameObject : EngineContext::GetGameScene ().GetGameObjectManager ())
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
    for (auto gameObject : EngineContext::GetGameScene ().GetGameObjectManager ())
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
    EngineContext::GetTimer ().Tick ();

    // process all game objects
    for (auto gameObject : EngineContext::GetGameScene ().GetGameObjectManager ())
    {
        gameObject->OnFrame ();
    }

    // render frame
    EngineContext::GetRenderer ().RenderFrame ();

    // process controller events
    EngineContext::GetInputController ().OnFrame ();

}



