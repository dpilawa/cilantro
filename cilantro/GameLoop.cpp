#include "GameLoop.h"

GameLoop::GameLoop (GameScene & scene, Renderer & renderer) :
	gameScene (scene), gameRenderer (renderer)
{
	LogMessage (__FUNCTION__) << "Engine starting";

	// set flags
	shouldStop = false;

	// pre-set game clocks
	Time::Tick ();

	// initialize all game objects
	for (auto gameObject : gameScene.GetGameObjects ())
	{
		gameObject->OnStart ();
	}

	// initialize renderer
	gameRenderer.Initialize (&scene);
}

GameLoop::~GameLoop ()
{
	// deinitialize renderer
	gameRenderer.Deinitialize ();
	
	// deinitialize all game objects
	for (auto gameObject : gameScene.GetGameObjects ())
	{
		gameObject->OnEnd ();
	}

	LogMessage (__FUNCTION__) << "Engine stopped";

}

void GameLoop::Go ()
{
	// run game loop, terminate when shouldStop condition is met
	while (shouldStop != true) {

		// update game clocks (Tick)
		Time::Tick ();

		// update all game objects
		for (auto gameObject : gameScene.GetGameObjects ())
		{
			gameObject->OnFrame ();
		}

		// render frame
		gameRenderer.RenderFrame ();
	}
}

GameScene & GameLoop::GetScene ()
{
	return gameScene;
}

Renderer & GameLoop::GetRenderer ()
{
	return gameRenderer;
}

