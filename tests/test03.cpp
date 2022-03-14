#include "cilantroengine.h"
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
#include "graphics/QuadRenderStage.h"
#include "graphics/GLFWRenderer.h"
#include "graphics/Renderer.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"
#include "system/Game.h"
#include "system/Timer.h"

#include "ControlledCamera.h"

#include <iostream>

int main (int argc, char* argv[])
{
    Game::Initialize ();

    GameScene& gameScene = Game::CreateGameScene<GameScene> ("scene");
    CGLFWRenderer& renderer = gameScene.CreateRenderer<CGLFWRenderer> (1920, 1080, false, "Test 03", false, true, true);
    InputController& inputController = Game::CreateInputController<GLFWInputController> ();

    AssimpModelLoader modelLoader;

    renderer.AddRenderStage<CQuadRenderStage> ("hdr_postprocess").SetShaderProgram ("post_hdr_shader").SetPipelineFramebufferInputLink (EPipelineLink::LINK_PREVIOUS);
    renderer.AddRenderStage<CQuadRenderStage> ("fxaa_postprocess").SetShaderProgram ("post_fxaa_shader").SetRenderStageParameterFloat ("fMaxSpan", 4.0f).SetRenderStageParameterVector2f ("vInvResolution", Vector2f (1.0f / renderer.GetWidth (), 1.0f / renderer.GetHeight ())).SetPipelineFramebufferInputLink (EPipelineLink::LINK_PREVIOUS);
    renderer.AddRenderStage<CQuadRenderStage> ("gamma_postprocess+screen").SetShaderProgram ("post_gamma_shader").SetRenderStageParameterFloat ("fGamma", 2.1f).SetPipelineFramebufferInputLink (EPipelineLink::LINK_PREVIOUS).SetFramebufferEnabled (false);    

    inputController.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    inputController.BindInputEvent ("exit", [ & ]() { Game::Stop (); });

    inputController.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
    inputController.BindInputEvent ("mousemode", [ & ]() { inputController.SetMouseGameMode (!inputController.IsGameMode ()); });

    modelLoader.Load ("scene", "assets/Drunk Idle.fbx");

    ControlledCamera& cam = gameScene.AddGameObject<ControlledCamera> ("camera", 60.0f, 0.1f, 100.0f, 5.0f, 0.1f);
    cam.Initialize ();
    cam.GetLocalTransform ().Translate (0.0f, 100.0f, 250.0f);
    gameScene.SetActiveCamera ("camera");

    PointLight& light = gameScene.AddGameObject<PointLight> ("light");
    light.GetLocalTransform ().Translate (100.0f, 100.0f, 100.0f);
    light.SetColor (Vector3f (1.0f, 1.0f, 1.0f));
    light.SetEnabled (true);

    AnimationObject& anim = gameScene.GetGameObjectManager ().GetByName<AnimationObject> ("mixamo.com");
    anim.SetLooping (true);
    anim.Play();

    Game::Run ();

    Game::Deinitialize ();

    return 0;
}

