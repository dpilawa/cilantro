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
#include <filesystem>

int main (int argc, char* argv [])
{
    CGame::Initialize (std::filesystem::current_path ().string ());

    CGameScene& gameScene = CGame::Create<CGameScene> ("scene");
    CGLFWRenderer& renderer = gameScene.Create<CGLFWRenderer> (800, 600, true, false, "Test 04", false, true, true);
    InputController& inputController = CGame::Create<GLFWInputController> ();

    AssimpModelLoader modelLoader;

    renderer.Create<CQuadRenderStage> ("hdr_postprocess").SetShaderProgram ("post_hdr_shader").SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS);
    renderer.Create<CQuadRenderStage> ("fxaa_postprocess").SetShaderProgram ("post_fxaa_shader").SetRenderStageParameterFloat ("fMaxSpan", 4.0f).SetRenderStageParameterVector2f ("vInvResolution", Vector2f (1.0f / renderer.GetWidth (), 1.0f / renderer.GetHeight ())).SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS);   
    renderer.Create<CQuadRenderStage> ("gamma_postprocess+screen").SetShaderProgram ("post_gamma_shader").SetRenderStageParameterFloat ("fGamma", 2.1f).SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS).SetFramebufferEnabled (false);

    modelLoader.Load ("scene", "assets/Cerberus_LP.FBX");
    CGame::GetResourceManager ().Load<Texture> ("tAlbedo", "assets/Textures/Cerberus_A.tga");
    CGame::GetResourceManager ().Load<Texture> ("tNormal", "assets/Textures/Cerberus_N.tga");
    CGame::GetResourceManager ().Load<Texture> ("tMetalness", "assets/Textures/Cerberus_M.tga");
    CGame::GetResourceManager ().Load<Texture> ("tRoughness", "assets/Textures/Cerberus_R.tga");
  
    inputController.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    inputController.BindInputEvent ("exit", [ & ]() { CGame::Stop (); });

    inputController.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
    inputController.BindInputEvent ("mousemode", [ & ]() { inputController.SetMouseGameMode (!inputController.IsGameMode ()); });

    PBRMaterial& m = gameScene.Add<PBRMaterial> ("gunMaterial");
    m.SetAlbedo ("tAlbedo").SetNormal ("tNormal").SetMetallic ("tMetalness").SetRoughness ("tRoughness");

    MeshObject& gun = gameScene.GetGameObjectManager ().GetByName<MeshObject> ("Cerberus00_Fixed");
    gun.SetMaterial ("gunMaterial");
    gun.GetLocalTransform ().Rotate (0.0f, 0.0f, 135.0f);

    ControlledCamera& cam = gameScene.Add<ControlledCamera> ("camera", 60.0f, 10.0f, 250.0f, 1.5f, 0.1f);
    cam.Initialize ();
    cam.GetLocalTransform ().Translate (0.0f, 0.0f, 50.0f);
    gameScene.SetActiveCamera ("camera");

    DirectionalLight& light = gameScene.Add<DirectionalLight> ("light");
    light.GetLocalTransform ().Rotate (45.0f, -120.0f, 0.0f);
    light.SetColor (Vector3f (2.5f, 2.1f, 1.7f));
    light.SetEnabled (true);

    CGame::Run ();

    CGame::Deinitialize ();

    return 0;
}

