#include "cilantroengine.h"
#include "game/Game.h"
#include "scene/GameScene.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "scene/PBRMaterial.h"
#include "resource/ResourceManager.h"
#include "resource/AssimpModelLoader.h"
#include "graphics/GLDeferredGeometryRenderStage.h"
#include "graphics/GLForwardGeometryRenderStage.h"
#include "graphics/GLQuadRenderStage.h"
#include "graphics/Renderer.h"
#include "graphics/GLFWRenderTarget.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"
#include "system/LogMessage.h"
#include "system/EngineContext.h"
#include "system/Timer.h"
#include "ControlledCamera.h"

int main (int argc, char* argv [])
{
    ResourceManager resourceManager;
    GameScene gameScene;
    GLFWRenderTarget renderTarget ("Test 4", 800, 600, false, true, true);
    Renderer renderer (800, 600);
    GLFWInputController inputController;
    Timer timer;
    Game game;

    AssimpModelLoader modelLoader;

    EngineContext::Set (game, resourceManager, timer, gameScene, renderer, renderTarget, inputController);
    EngineContext::Initialize ();

    renderer.AddRenderStage<GLDeferredGeometryRenderStage> ("base");
    renderer.AddRenderStage<GLQuadRenderStage> ("hdr_postprocess").SetShaderProgram ("post_hdr_shader").SetPipelineFramebufferInputLink (PipelineLink::LINK_PREVIOUS);
    renderer.AddRenderStage<GLQuadRenderStage> ("fxaa_postprocess").SetShaderProgram ("post_fxaa_shader").SetRenderStageParameterFloat ("fMaxSpan", 4.0f).SetRenderStageParameterVector2f ("vInvResolution", Vector2f (1.0f / EngineContext::GetRenderer ().GetWidth (), 1.0f / EngineContext::GetRenderer ().GetHeight ())).SetPipelineFramebufferInputLink (PipelineLink::LINK_PREVIOUS);   
    renderer.AddRenderStage<GLQuadRenderStage> ("gamma_postprocess").SetShaderProgram ("post_gamma_shader").SetRenderStageParameterFloat ("fGamma", 1.7f).SetPipelineFramebufferInputLink (PipelineLink::LINK_PREVIOUS);

    modelLoader.Load ("assets/Cerberus_LP.FBX");
    resourceManager.Load<Texture> ("tAlbedo", "assets/Textures/Cerberus_A.tga");
    resourceManager.Load<Texture> ("tNormal", "assets/Textures/Cerberus_N.tga");
    resourceManager.Load<Texture> ("tMetalness", "assets/Textures/Cerberus_M.tga");
    resourceManager.Load<Texture> ("tRoughness", "assets/Textures/Cerberus_R.tga");
  
    inputController.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    inputController.BindInputEvent ("exit", [ & ]() { game.Stop (); });

    inputController.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
    inputController.BindInputEvent ("mousemode", [ & ]() { inputController.SetMouseGameMode (!inputController.IsGameMode ()); });

    PBRMaterial& m = gameScene.AddMaterial<PBRMaterial> ("gunMaterial");
    m.SetAlbedo ("tAlbedo").SetNormal ("tNormal").SetMetallic ("tMetalness").SetRoughness ("tRoughness");

    MeshObject& gun = gameScene.AddGameObject<MeshObject> ("gun", "Cerberus00_Fixed", "gunMaterial");
    gun.GetModelTransform ().Scale (0.1f).Rotate (-90.0f, 0.0f, 0.0f);

    ControlledCamera& cam = gameScene.AddGameObject<ControlledCamera> ("camera", 60.0f, 0.01f, 100.0f, 0.1f);
    cam.Initialize ();
    cam.GetModelTransform ().Translate (0.0f, 0.0f, 20.0f);
    gameScene.SetActiveCamera ("camera");

    DirectionalLight& light = gameScene.AddGameObject<DirectionalLight> ("light");
    light.GetModelTransform ().Rotate (45.0f, -135.0f, 0.0f);
    light.SetColor (Vector3f (5.0f, 4.2f, 3.3f));
    light.SetEnabled (true);

    game.Run ();

    EngineContext::Deinitialize ();

    return 0;
}

