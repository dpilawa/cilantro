#include "cilantroengine.h"
#include "game/Game.h"
#include "scene/Primitives.h"
#include "scene/AnimationObject.h"
#include "scene/GameScene.h"
#include "scene/PhongMaterial.h"
#include "scene/MeshObject.h"
#include "scene/PointLight.h"
#include "resource/ResourceManager.h"
#include "resource/AssimpModelLoader.h"
#include "graphics/GLDeferredGeometryRenderStage.h"
#include "graphics/GLForwardGeometryRenderStage.h"
#include "graphics/GLQuadRenderStage.h"
#include "graphics/GLFWRenderer.h"
#include "graphics/Renderer.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"
#include "system/EngineContext.h"
#include "system/Timer.h"

#include "ControlledCamera.h"

#include <iostream>

int main (int argc, char* argv[])
{
    ResourceManager resourceManager;
    GameScene gameScene;
    GLFWRenderer renderer (800, 600, "Test 03", false, true, true);
    GLFWInputController inputController;
    Timer timer;
    Game game;

    AssimpModelLoader modelLoader;

    EngineContext::Set (game, resourceManager, timer, gameScene, renderer, inputController);
    EngineContext::Initialize ();

    renderer.AddRenderStage<GLForwardGeometryRenderStage> ("base");
    renderer.AddRenderStage<GLQuadRenderStage> ("hdr_postprocess").SetShaderProgram ("post_hdr_shader").SetPipelineFramebufferInputLink (PipelineLink::LINK_PREVIOUS);
    renderer.AddRenderStage<GLQuadRenderStage> ("fxaa_postprocess").SetShaderProgram ("post_fxaa_shader").SetRenderStageParameterFloat ("fMaxSpan", 4.0f).SetRenderStageParameterVector2f ("vInvResolution", Vector2f (1.0f / EngineContext::GetRenderer ().GetWidth (), 1.0f / EngineContext::GetRenderer ().GetHeight ())).SetPipelineFramebufferInputLink (PipelineLink::LINK_PREVIOUS);
    renderer.AddRenderStage<GLQuadRenderStage> ("gamma_postprocess+screen").SetShaderProgram ("post_gamma_shader").SetRenderStageParameterFloat ("fGamma", 2.1f).SetPipelineFramebufferInputLink (PipelineLink::LINK_PREVIOUS).SetFramebufferEnabled (false);    

    inputController.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    inputController.BindInputEvent ("exit", [ & ]() { game.Stop (); });

    inputController.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
    inputController.BindInputEvent ("mousemode", [ & ]() { inputController.SetMouseGameMode (!inputController.IsGameMode ()); });

    modelLoader.Load ("assets/Drunk Idle.fbx");

    ControlledCamera& cam = gameScene.AddGameObject<ControlledCamera> ("camera", 60.0f, 0.1f, 100.0f, 5.0f, 0.1f);
    cam.Initialize ();
    cam.GetModelTransform ().Translate (0.0f, 100.0f, 250.0f);
    gameScene.SetActiveCamera ("camera");

    PointLight& light = gameScene.AddGameObject<PointLight> ("light");
    light.GetModelTransform ().Translate (100.0f, 100.0f, 100.0f);
    light.SetColor (Vector3f (1.0f, 1.0f, 1.0f));
    light.SetEnabled (true);

    game.Run ();

    EngineContext::Deinitialize ();

    return 0;
}

