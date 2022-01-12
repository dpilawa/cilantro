#ifndef _GAME_H_
#define _GAME_H_

#include "cilantroengine.h"
#include "system/Timer.h"
#include "resource/Resource.h"
#include "resource/ResourceManager.h"
#include "scene/GameScene.h"
#include "graphics/Renderer.h"
#include "input/InputController.h"

class Game
{
public:

    // initialize and deinitialize
    static __EAPI void Initialize (ResourceManager<Resource>& resourceManager, Timer& timer, GameScene& gameScene, Renderer& renderer, InputController& inputController);
    static __EAPI void Deinitialize ();

    // run a game loop
    static __EAPI void Run ();
    static __EAPI void Stop ();	
    static __EAPI void Step ();

    // accessors
    static __EAPI ResourceManager<Resource>& GetResourceManager ();
    static __EAPI Timer& GetTimer ();
    static __EAPI GameScene& GetGameScene ();
    static __EAPI Renderer& GetRenderer ();
    static __EAPI InputController& GetInputController ();

private:
    
    static ResourceManager<Resource>* contextResourceManager;
    static Timer* contextTimer;
    static GameScene* contextGameScene;
    static Renderer* contextRenderer;
    static InputController* contextInputController;

    // game state
    static bool shouldStop;
    static bool isRunning;

};

#endif

