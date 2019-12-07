#ifndef _GAMELOOP_H_
#define _GAMELOOP_H_

#include "cilantroengine.h"

class GameScene;
class Renderer;
class RenderTarget;
class InputController;
class GameLoop;

class GameLoop
{
public:

    __EAPI GameLoop ();
    __EAPI ~GameLoop ();

    // run a game loop
	__EAPI void Run ();
	__EAPI void Stop ();	
	__EAPI void Step ();

	// game loop components
    GameScene* gameScene;
    Renderer* gameRenderer;
	InputController* gameInputController;
    RenderTarget* gameRenderTarget;

private:
    // bool flags to control the game loop
	bool shouldStop;

};

#endif

