#include "GameLoop.h"
#include <chrono>
#include <thread>

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
