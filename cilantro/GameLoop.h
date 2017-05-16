#ifndef _GAMELOOP_H_
#define _GAMELOOP_H_

#include "GameScene.h"
#include "Renderer.h"
#include "Time.h"

class GameLoop
{
	// game loop requires a scene object
	// and renderer interface
	GameScene* gameScene;
	Renderer* gameRenderer;
	bool shouldStop = false;

public:
	GameLoop (GameScene& scene, Renderer& renderer) : gameScene (&scene), gameRenderer (&renderer) {};
	~GameLoop () {};

	// run a game loop
	void Go ();
};

#endif

