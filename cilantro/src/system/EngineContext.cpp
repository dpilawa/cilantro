#include "system/EngineContext.h"

ResourceManager<Resource>* EngineContext::contextResourceManager;
Game* EngineContext::contextGame;
Timer* EngineContext::contextTimer;
GameScene* EngineContext::contextGameScene;
Renderer* EngineContext::contextRenderer;
InputController* EngineContext::contextInputController;

void EngineContext::Set (Game& game, ResourceManager<Resource>& resourceManager, Timer& timer, GameScene& gameScene, Renderer& renderer, InputController& inputController)
{
    contextGame = &game;
    contextResourceManager = &resourceManager;
    contextTimer = &timer;
    contextGameScene = &gameScene;
    contextRenderer = &renderer;
    contextInputController = &inputController;
}

void EngineContext::Initialize ()
{
    contextRenderer->Initialize ();
    contextInputController->Initialize ();
    contextGame->Initialize ();
}

void EngineContext::Deinitialize ()
{
    contextGame->Deinitialize ();
    contextInputController->Deinitialize ();
    contextRenderer->Deinitialize ();
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

InputController& EngineContext::GetInputController ()
{
    return *contextInputController;
}