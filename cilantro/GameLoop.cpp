#include "GameLoop.h"
#include <iostream>

GameLoop::GameLoop (GameScene & scene, Renderer & renderer) :
	gameScene (scene), gameRenderer (renderer)
{
	// set flags
	shouldStop = false;
	// pre-set game clocks
	Time::Tick ();
	// initialize renderer
	gameRenderer.OnStart ();
	// initialize all game objects
	// ...
}

GameLoop::~GameLoop ()
{
	// deinitialize renderer
	gameRenderer.OnEnd ();
	// deinitialize all game objects
	// ...
}

void GameLoop::Go ()
{
	// run game loop, terminate when shouldStop condition is met
	while (shouldStop != true) {
		// update game clocks
		Time::Tick ();
		// render frame
		gameRenderer.OnFrame ();
	}

}

