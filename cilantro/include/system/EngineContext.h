#ifndef _ENGINECONTEXT_H_
#define _ENGINECONTEXT_H_

#include "cilantroengine.h"
#include "system/Timer.h"
#include "scene/GameScene.h"
#include "graphics/Renderer.h"
#include "graphics/RenderTarget.h"
#include "input/InputController.h"

class EngineContext
{
public:

    // initialize engine context
    static __EAPI void Set (Timer& timer, GameScene& gameScene, Renderer& renderer, RenderTarget& renderTarget, InputController& inputController);

    // accessors
    static __EAPI Timer& GetTimer ();
    static __EAPI GameScene& GetGameScene ();
    static __EAPI Renderer& GetRenderer ();
    static __EAPI RenderTarget& GetRenderTarget ();
    static __EAPI InputController& GetInputController ();

private:
    
    static Timer* contextTimer;
    static GameScene* contextGameScene;
    static Renderer* contextRenderer;
    static RenderTarget* contextRenderTarget;
    static InputController* contextInputController;
    
};

#endif
