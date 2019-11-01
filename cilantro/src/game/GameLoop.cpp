#include "cilantroengine.h"
#include "game/GameLoop.h"
#include "scene/GameScene.h"
#include "graphics/Renderer.h"
#include "input/InputController.h"
#include "util/LogMessage.h"
#include "util/Timer.h"

void GameLoopChild::AttachToGame (GameLoop* parentGameLoop)
{
    gameLoop = parentGameLoop;
}

GameLoop::GameLoop (GameScene& scene, InputController& inputController, Renderer& renderer, RenderTarget& renderTarget) :
	gameScene (scene), gameRenderer (renderer), gameInputController (inputController), gameRenderTarget(renderTarget)
{
	LogMessage (__func__) << "Engine starting";

	// set flags
	shouldStop = false;

	// pre-set game clocks
	Timer::Tick ();

	// initialize all game objects
	for (auto gameObject : gameScene.GetGameObjects ())
	{
		gameObject.second->OnStart ();
	}

	// set parent relationships
    gameScene.AttachToGame (this);
    gameRenderer.AttachToGame (this);
    gameRenderTarget.AttachToGame (this);
    gameInputController.AttachToGame (this);

	// initialize renderer & controller
    gameRenderTarget.Initialize ();
    gameRenderer.Initialize ();
    gameInputController.Initialize ();

}

GameLoop::~GameLoop ()
{
	// deinitialize renderer & controller
	gameInputController.Deinitialize ();
	gameRenderer.Deinitialize ();
    gameRenderTarget.Deinitialize ();

    // deinitialize all game objects
	for (auto gameObject : gameScene.GetGameObjects ())
	{
        gameObject.second->OnEnd ();
    }

    LogMessage (__func__) << "Engine stopped";

}

void GameLoop::Run ()
{
	// run game loop, terminate when shouldStop condition is met
	while (shouldStop != true) {
		Step ();
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

    // update all game objects
	for (auto gameObject : gameScene.GetGameObjects ())
	{
		gameObject.second->OnFrame ();
	}

	// process controller events
	gameInputController.OnFrame ();

	// render frame
	gameRenderer.RenderFrame ();
}

GameScene& GameLoop::GetScene ()
{
	return gameScene;
}

RenderTarget& GameLoop::GetRenderTarget ()
{
	return gameRenderTarget;
}

Renderer& GameLoop::GetRenderer ()
{
	return gameRenderer;
}

InputController& GameLoop::GetInputController ()
{
	return gameInputController;
}


