#ifndef _GAMELOOP_H_
#define _GAMELOOP_H_

#include "cilantroengine.h"
#include "LogMessage.h"
#include "GameScene.h"
#include "Renderer.h"
#include "Time.h"
#include <string>

class GameLoop
{
public:
	__EAPI GameLoop (GameScene& scene, Renderer& renderer);
	__EAPI ~GameLoop ();

	// run a game loop
	__EAPI void Run ();
	__EAPI void Step ();

	// get scene reference
	__EAPI GameScene& GetScene ();

	// get renderer reference
	__EAPI Renderer& GetRenderer ();

private:

	// game loop requires a scene object
	// and renderer interface
	GameScene& gameScene;
	Renderer& gameRenderer;

	// bool flags to control the game loop
	bool shouldStop;

};

#endif

