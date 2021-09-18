#ifndef _ENGINECONTEXT_H_
#define _ENGINECONTEXT_H_

#include "cilantroengine.h"
#include "system/Timer.h"
#include "resource/Resource.h"
#include "resource/ResourceManager.h"
#include "scene/GameScene.h"
#include "graphics/Renderer.h"
#include "input/InputController.h"
#include "game/Game.h"

class EngineContext
{
public:

    // initialize engine context
    static __EAPI void Set (Game& game, ResourceManager<Resource>& resourceManager, Timer& timer, GameScene& gameScene, Renderer& renderer, InputController& inputController);

    // initialize and deinitialize
    static __EAPI void Initialize ();
    static __EAPI void Deinitialize ();

    // accessors
    static __EAPI Game& GetGame();
    static __EAPI ResourceManager<Resource>& GetResourceManager ();
    static __EAPI Timer& GetTimer ();
    static __EAPI GameScene& GetGameScene ();
    static __EAPI Renderer& GetRenderer ();
    static __EAPI InputController& GetInputController ();

private:
    
    static Game* contextGame;
    static ResourceManager<Resource>* contextResourceManager;
    static Timer* contextTimer;
    static GameScene* contextGameScene;
    static Renderer* contextRenderer;
    static InputController* contextInputController;
    
};

#endif
