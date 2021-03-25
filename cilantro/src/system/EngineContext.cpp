#include "system/EngineContext.h"

ResourceManager<Resource>* EngineContext::contextResourceManager;
Game* EngineContext::contextGame;
Timer* EngineContext::contextTimer;
GameScene* EngineContext::contextGameScene;
Renderer* EngineContext::contextRenderer;
RenderTarget* EngineContext::contextRenderTarget;
InputController* EngineContext::contextInputController;

void EngineContext::Set (Game& game, ResourceManager<Resource>& resourceManager, Timer& timer, GameScene& gameScene, Renderer& renderer, RenderTarget& renderTarget, InputController& inputController)
{
    contextGame = &game;
    contextResourceManager = &resourceManager;
    contextTimer = &timer;
    contextGameScene = &gameScene;
    contextRenderer = &renderer;
    contextRenderTarget = &renderTarget;
    contextInputController = &inputController;
}

void EngineContext::Initialize ()
{
    contextRenderTarget->Initialize ();
    contextRenderer->Initialize ();
    contextInputController->Initialize ();
}

void EngineContext::Deinitialize ()
{
    contextInputController->Deinitialize ();
    contextRenderer->Deinitialize ();
    contextRenderTarget->Deinitialize ();
}

Game& EngineContext::GetGame ()
{
    return *contextGame;
}

ResourceManager<Resource>& EngineContext::GetResourceManager ()
{
    return *contextResourceManager;
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