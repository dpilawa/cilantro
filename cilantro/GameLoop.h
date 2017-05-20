#ifndef _GAMELOOP_H_
#define _GAMELOOP_H_

#include "GameScene.h"
#include "Renderer.h"
#include "Time.h"
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>

class GameLoop
{
public:
	GameLoop (GameScene& scene, Renderer& renderer);
	~GameLoop () {};

	// run a game loop
	void Go ();

private:

	// game loop requires a scene object
	// and renderer interface
	GameScene* gameScene;
	Renderer* gameRenderer;

	// bool flags to control the game loop
	std::atomic_bool shouldStop;
};

#endif

