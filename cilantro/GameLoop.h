#ifndef _GAMELOOP_H_
#define _GAMELOOP_H_

#include "cilantroengine.h"

class GameScene;
class Renderer;
class RenderTarget;
class InputController;
class GameLoop;

class GameLoopChild
{
public:
    GameLoopChild () : gameLoop (nullptr) {};
    ~GameLoopChild () {};

    void AttachToGame (GameLoop* parentGameLoop);

protected:

    GameLoop* gameLoop;
};

class GameLoop
{
public:

    __EAPI GameLoop (GameScene& scene, InputController& inputController, Renderer& renderer, RenderTarget& renderTarget);
    __EAPI ~GameLoop ();

    // run a game loop
	__EAPI void Run ();
	__EAPI void Stop ();	
	__EAPI void Step ();

	// get scene reference
	__EAPI GameScene& GetScene ();

	// get renderer target reference
	__EAPI RenderTarget& GetRenderTarget ();

	// get renderer reference
	__EAPI Renderer& GetRenderer ();

	// get input controller reference
	__EAPI InputController& GetInputController ();

private:

	// game loop requires a scene object
	// and renderer interface
    GameScene& gameScene;
    Renderer& gameRenderer;
    RenderTarget& gameRenderTarget;
    InputController& gameInputController;

    // bool flags to control the game loop
	bool shouldStop;

};

#endif

