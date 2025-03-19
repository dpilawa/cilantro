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

using namespace cilantro;

int main (int argc, char* argv [])
{
    Game::Initialize (std::filesystem::current_path ().string ());

    GameScene& gameScene = Game::Create<GameScene> ("scene");
    GLFWRenderer& renderer = gameScene.Create<GLFWRenderer> (800, 600, true, false, "Test 04", false, true, true);
    InputController& inputController = Game::Create<GLFWInputController> ();

    AssimpModelLoader modelLoader;

    renderer.Create<QuadRenderStage> ("hdr_postprocess").SetShaderProgram ("post_hdr_shader").SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS);
    renderer.Create<QuadRenderStage> ("fxaa_postprocess").SetShaderProgram ("post_fxaa_shader").SetRenderStageParameterFloat ("fMaxSpan", 4.0f).SetRenderStageParameterVector2f ("vInvResolution", Vector2f (1.0f / renderer.GetWidth (), 1.0f / renderer.GetHeight ())).SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS);   
    renderer.Create<QuadRenderStage> ("gamma_postprocess+screen").SetShaderProgram ("post_gamma_shader").SetRenderStageParameterFloat ("fGamma", 2.1f).SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS).SetFramebufferEnabled (false);

    modelLoader.Load ("scene", "assets/Cerberus_LP.FBX");
    Game::GetResourceManager ().Load<Texture> ("tAlbedo", "assets/Textures/Cerberus_A.tga");
    Game::GetResourceManager ().Load<Texture> ("tNormal", "assets/Textures/Cerberus_N.tga");
    Game::GetResourceManager ().Load<Texture> ("tMetalness", "assets/Textures/Cerberus_M.tga");
    Game::GetResourceManager ().Load<Texture> ("tRoughness", "assets/Textures/Cerberus_R.tga");
  
    inputController.CreateInputEvent ("exit", EInputKey::KeyEsc, EInputTrigger::Press, {});
    inputController.BindInputEvent ("exit", [ & ]() { Game::Stop (); });

    inputController.CreateInputEvent ("mousemode", EInputKey::KeySpace, EInputTrigger::Release, {});
    inputController.BindInputEvent ("mousemode", [ & ]() { inputController.SetMouseGameMode (!inputController.IsGameMode ()); });

    PBRMaterial& m = gameScene.Create<PBRMaterial> ("gunMaterial");
    m.SetAlbedo ("tAlbedo").SetNormal ("tNormal").SetMetallic ("tMetalness").SetRoughness ("tRoughness");

    MeshObject& gun = gameScene.GetGameObjectManager ().GetByName<MeshObject> ("Cerberus00_Fixed");
    gun.SetMaterial ("gunMaterial");
    gun.GetLocalTransform ().Rotate (0.0f, 0.0f, 135.0f);

    ControlledCamera& cam = gameScene.Create<ControlledCamera> ("camera", 60.0f, 10.0f, 250.0f, 1.5f, 0.1f);
    cam.Initialize ();
    cam.GetLocalTransform ().Translate (0.0f, 0.0f, 50.0f);
    gameScene.SetActiveCamera ("camera");

    DirectionalLight& light = gameScene.Create<DirectionalLight> ("light");
    light.GetLocalTransform ().Rotate (45.0f, -120.0f, 0.0f);
    light.SetColor (Vector3f (2.5f, 2.1f, 1.7f));
    light.SetEnabled (true);

    Game::Run ();

    Game::Deinitialize ();

    return 0;
}

