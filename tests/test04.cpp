#include "cilantroengine.h"
#include "scene/GameScene.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "scene/PBRMaterial.h"
#include "resource/ResourceManager.h"
#include "resource/AssimpModelLoader.h"
#include "graphics/QuadRenderStage.h"
#include "graphics/Renderer.h"
#include "graphics/GLFWRenderer.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"
#include "system/LogMessage.h"
#include "system/Game.h"
#include "system/Timer.h"
#include "ControlledCamera.h"

int main (int argc, char* argv [])
{
    CGame::Initialize ();

    CGameScene& gameScene = CGame::CreateGameScene<CGameScene> ("scene");
    CGLFWRenderer& renderer = gameScene.CreateRenderer<CGLFWRenderer> (800, 600, true, "Test 01", false, true, true);
    InputController& inputController = CGame::CreateInputController<GLFWInputController> ();

    AssimpModelLoader modelLoader;

    renderer.AddRenderStage<CQuadRenderStage> ("hdr_postprocess").SetShaderProgram ("post_hdr_shader").SetPipelineFramebufferInputLink (EPipelineLink::LINK_PREVIOUS);
    renderer.AddRenderStage<CQuadRenderStage> ("fxaa_postprocess").SetShaderProgram ("post_fxaa_shader").SetRenderStageParameterFloat ("fMaxSpan", 4.0f).SetRenderStageParameterVector2f ("vInvResolution", Vector2f (1.0f / renderer.GetWidth (), 1.0f / renderer.GetHeight ())).SetPipelineFramebufferInputLink (EPipelineLink::LINK_PREVIOUS);   
    renderer.AddRenderStage<CQuadRenderStage> ("gamma_postprocess+screen").SetShaderProgram ("post_gamma_shader").SetRenderStageParameterFloat ("fGamma", 2.1f).SetPipelineFramebufferInputLink (EPipelineLink::LINK_PREVIOUS).SetFramebufferEnabled (false);

    modelLoader.Load ("scene", "assets/Cerberus_LP.FBX");
    CGame::GetResourceManager ().Load<Texture> ("tAlbedo", "assets/Textures/Cerberus_A.tga");
    CGame::GetResourceManager ().Load<Texture> ("tNormal", "assets/Textures/Cerberus_N.tga");
    CGame::GetResourceManager ().Load<Texture> ("tMetalness", "assets/Textures/Cerberus_M.tga");
    CGame::GetResourceManager ().Load<Texture> ("tRoughness", "assets/Textures/Cerberus_R.tga");
  
    inputController.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    inputController.BindInputEvent ("exit", [ & ]() { CGame::Stop (); });

    inputController.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
    inputController.BindInputEvent ("mousemode", [ & ]() { inputController.SetMouseGameMode (!inputController.IsGameMode ()); });

    PBRMaterial& m = gameScene.AddMaterial<PBRMaterial> ("gunMaterial");
    m.SetAlbedo ("tAlbedo").SetNormal ("tNormal").SetMetallic ("tMetalness").SetRoughness ("tRoughness");

    MeshObject& gun = gameScene.GetGameObjectManager ().GetByName<MeshObject> ("Cerberus00_Fixed");
    gun.SetMaterial ("gunMaterial");
    gun.GetLocalTransform ().Scale (0.8f).Rotate (0.0f, 15.0f, 0.0f);

    ControlledCamera& cam = gameScene.AddGameObject<ControlledCamera> ("camera", 60.0f, 0.01f, 100.0f, 0.1f, 0.1f);
    cam.Initialize ();
    cam.GetLocalTransform ().Translate (0.0f, 0.0f, 20.0f);
    gameScene.SetActiveCamera ("camera");

    DirectionalLight& light = gameScene.AddGameObject<DirectionalLight> ("light");
    light.GetLocalTransform ().Rotate (45.0f, -135.0f, 0.0f);
    light.SetColor (Vector3f (2.5f, 2.1f, 1.7f));
    light.SetEnabled (true);

    CGame::Run ();

    CGame::Deinitialize ();

    return 0;
}

