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
#include "graphics/GLQuadRenderStage.h"
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
    ResourceManager resourceManager;
    GameScene gameScene;
    GLFWRenderer renderer (1920, 1080, "Test 03", false, true, true);
    GLFWInputController inputController;
    Timer timer;

    AssimpModelLoader modelLoader;

    Game::Initialize (resourceManager, timer, gameScene, renderer, inputController);

    renderer.AddRenderStage<GLForwardGeometryRenderStage> ("base");
    renderer.AddRenderStage<GLQuadRenderStage> ("hdr_postprocess").SetShaderProgram ("post_hdr_shader").SetPipelineFramebufferInputLink (PipelineLink::LINK_PREVIOUS);
    renderer.AddRenderStage<GLQuadRenderStage> ("fxaa_postprocess").SetShaderProgram ("post_fxaa_shader").SetRenderStageParameterFloat ("fMaxSpan", 4.0f).SetRenderStageParameterVector2f ("vInvResolution", Vector2f (1.0f / Game::GetRenderer ().GetWidth (), 1.0f / Game::GetRenderer ().GetHeight ())).SetPipelineFramebufferInputLink (PipelineLink::LINK_PREVIOUS);
    renderer.AddRenderStage<GLQuadRenderStage> ("gamma_postprocess+screen").SetShaderProgram ("post_gamma_shader").SetRenderStageParameterFloat ("fGamma", 2.1f).SetPipelineFramebufferInputLink (PipelineLink::LINK_PREVIOUS).SetFramebufferEnabled (false);    

    inputController.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    inputController.BindInputEvent ("exit", [ & ]() { Game::Stop (); });

    inputController.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
    inputController.BindInputEvent ("mousemode", [ & ]() { inputController.SetMouseGameMode (!inputController.IsGameMode ()); });

    modelLoader.Load ("assets/Drunk Idle.fbx");

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

