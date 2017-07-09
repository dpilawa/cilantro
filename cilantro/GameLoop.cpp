#include "GameLoop.h"
#include <iostream>

GameLoop::GameLoop (GameScene & scene, Renderer & renderer) :
	gameScene (scene), gameRenderer (renderer)
{
	// set flags
	shouldStop = false;

	// pre-set game clocks
	Time::Tick ();

	// initialize all game objects
	for (auto gameObject : gameScene.GetGameObjects ())
	{
		gameObject->OnStart ();
	}

	// set pointer to a game loop in a scene
	scene.SetGameLoop (*this);

	// initialize renderer
	gameRenderer.Initialize ();
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

		// draw all game objects
		//for (auto gameObject : gameScene.GetGameObjects ())
		//{
		//	gameObject->OnDraw (gameRenderer);
		//}

		// render frame
		gameRenderer.RenderFrame ();

		// display debug message
		if (gameRenderer.GetFrameCount() % 200 == 0)
		{
			LogMessage () << "Actual FPS:" << 1 / Time::GetFrameDeltaTime () << "; Theoretical FPS:" << 1 / Time::GetFrameRenderTime ();
		}
	
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

