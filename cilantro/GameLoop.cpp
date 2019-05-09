#include "cilantroengine.h"
#include "GameLoop.h"
#include "GameScene.h"
#include "Renderer.h"
#include "InputController.h"
#include "LogMessage.h"
#include "Timer.h"

GameLoop::GameLoop (GameScene & scene, InputController & inputController, Renderer & renderer) :
	gameScene (scene), gameInputController (inputController), gameRenderer (renderer)
{
	LogMessage (__func__) << "Engine starting";

	// set flags
	shouldStop = false;

	// pre-set game clocks
	Timer::Tick ();

	// initialize all game objects
	for (auto gameObject : gameScene.GetGameObjects ())
	{
		gameObject->OnStart ();
	}

	// initialize renderer & controller
	gameRenderer.Initialize ();
	gameInputController.Initialize ();
}

GameLoop::~GameLoop ()
{
	// deinitialize renderer & controller
	gameInputController.Deinitialize ();
	gameRenderer.Deinitialize ();

	// deinitialize all game objects
	for (auto gameObject : gameScene.GetGameObjects ())
	{
		gameObject->OnEnd ();
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
		gameObject->OnFrame ();
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

Renderer& GameLoop::GetRenderer ()
{
	return gameRenderer;
}

