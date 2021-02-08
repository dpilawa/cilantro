#ifndef _GAME_H_
#define _GAME_H_

#include "cilantroengine.h"

class GameScene;
class Resource;
template <typename Base> class ResourceManager;
class Renderer;
class RenderTarget;
class InputController;

class Game
{
public:

    __EAPI Game (ResourceManager<Resource>& resourceManager, GameScene& gameScene, Renderer& renderer, RenderTarget& renderTarget, InputController& inputController);
    __EAPI ~Game ();

    // run a game loop
    __EAPI void Run ();
    __EAPI void Stop ();	
    __EAPI void Step ();

    __EAPI ResourceManager<Resource>& GetResourceManager ();
    __EAPI GameScene& GetGameScene ();
    __EAPI Renderer& GetRenderer ();
    __EAPI RenderTarget& GetRenderTarget ();
    __EAPI InputController& GetInputController ();

private:

    // game composites
    ResourceManager<Resource>& resourceManager;
    GameScene& gameScene;
    Renderer& renderer;
    RenderTarget& renderTarget;
    InputController& inputController;

    // game state
    bool shouldStop;
    bool isRunning;

};

#endif

