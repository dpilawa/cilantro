#include "system/EngineContext.h"

Timer* EngineContext::contextTimer;
GameScene* EngineContext::contextGameScene;
Renderer* EngineContext::contextRenderer;
RenderTarget* EngineContext::contextRenderTarget;
InputController* EngineContext::contextInputController;

void EngineContext::Set (Timer& timer, GameScene& gameScene, Renderer& renderer, RenderTarget& renderTarget, InputController& inputController)
{
    contextTimer = &timer;
    contextGameScene = &gameScene;
    contextRenderer = &renderer;
    contextRenderTarget = &renderTarget;
    contextInputController = &inputController;
}

Timer& EngineContext::GetTimer ()
{
    return *contextTimer;
}

GameScene& EngineContext::GetGameScene ()
{
    return *contextGameScene;
}

Renderer& EngineContext::GetRenderer ()
{
    return *contextRenderer;
}

RenderTarget& EngineContext::GetRenderTarget ()
{
    return *contextRenderTarget;
}

InputController& EngineContext::GetInputController ()
{
    return *contextInputController;
}