#include "GameLoop.h"
#include <iostream>

GameLoop::GameLoop (GameScene & scene, Renderer & renderer) :
	gameScene (&scene), gameRenderer (&renderer)
{
	shouldStop = false;
}

void GameLoop::Go ()
{
	// pre-set game clocks
	Time::Tick ();

	// initialize renderer
	gameRenderer->OnStart ();
	
	// run game loop, terminate when shouldStop condition is met
	while (shouldStop != true) {
		Time::Tick ();
		gameRenderer->OnFrame ();
	}

	// deinitialize renderer
	gameRenderer->OnEnd ();
}

