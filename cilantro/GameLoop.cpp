#include "GameLoop.h"

GameLoop::GameLoop (GameScene & scene, Renderer & renderer, const float renderFPS) :
	gameScene (&scene), gameRenderer (&renderer), renderFPS (renderFPS)
{
	renderFrameDuration = std::chrono::duration<int, std::micro> ((int)(1000000.f / renderFPS));
}

void GameLoop::Go ()
{
	std::chrono::duration<int, std::micro> timer_duration2 (16667);

	// set game clocks
	Time::Tick ();

	// initialize renderer
	gameRenderer->OnStart ();

	// run game loop, terminate when shouldStop condition is met
	while (shouldStop != true) {
		gameRenderer->OnFrame ();
		std::this_thread::sleep_for (timer_duration2);
	}

	// deinitialize renderer
	gameRenderer->OnEnd ();
}
