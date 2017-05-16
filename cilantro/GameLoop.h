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
	GameLoop (GameScene& scene, Renderer& renderer, const float renderFPS);
	~GameLoop () {};

	// run a game loop
	void Go ();

private:

	// game loop requires a scene object
	// and renderer interface
	GameScene* gameScene;
	Renderer* gameRenderer;

	// frames per second
	float renderFPS;
	std::chrono::duration<int, std::micro> renderFrameDuration;

	// bool flags to control the loop and fps synchronization
	std::atomic_bool shouldStop = false;
	std::atomic_bool frameInProgress = false;
};

#endif

