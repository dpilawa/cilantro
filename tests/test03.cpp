#include "cilantroengine.h"
#include "scene/Primitives.h"
#include "scene/AnimationObject.h"
#include "scene/GameScene.h"
#include "scene/PhongMaterial.h"
#include "scene/PBRMaterial.h"
#include "scene/MeshObject.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "resource/ResourceManager.h"
#include "resource/AssimpModelLoader.h"
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
    CGame::Initialize ();

    CGameScene& gameScene = CGame::CreateGameScene<CGameScene> ("scene");
    CGLFWRenderer& renderer = gameScene.CreateRenderer<CGLFWRenderer> (1920, 1080, true, true, "Test 03", false, true, true);
    InputController& inputController = CGame::CreateInputController<GLFWInputController> ();

    AssimpModelLoader modelLoader;

    renderer.AddRenderStage<CQuadRenderStage> ("hdr_postprocess").SetShaderProgram ("post_hdr_shader").SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS);
    renderer.AddRenderStage<CQuadRenderStage> ("fxaa_postprocess").SetShaderProgram ("post_fxaa_shader").SetRenderStageParameterFloat ("fMaxSpan", 4.0f).SetRenderStageParameterVector2f ("vInvResolution", Vector2f (1.0f / renderer.GetWidth (), 1.0f / renderer.GetHeight ())).SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS);
    renderer.AddRenderStage<CQuadRenderStage> ("gamma_postprocess+screen").SetShaderProgram ("post_gamma_shader").SetRenderStageParameterFloat ("fGamma", 2.1f).SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS).SetFramebufferEnabled (false);    

    inputController.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    inputController.BindInputEvent ("exit", [ & ]() { CGame::Stop (); });

    inputController.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
    inputController.BindInputEvent ("mousemode", [ & ]() { inputController.SetMouseGameMode (!inputController.IsGameMode ()); });

    modelLoader.Load ("scene", "assets/Drunk Idle.fbx");

    gameScene.AddMaterial<PBRMaterial> ("floorMaterial").SetAlbedo (Vector3f (0.3f, 0.2f, 0.2f)).SetRoughness (0.1f).SetMetallic (0.6f);

    Mesh& floorMesh = CGame::GetResourceManager ().Create<Mesh> ("floorMesh");
    Primitives::GenerateCube (floorMesh);
    MeshObject& floor = gameScene.AddGameObject<MeshObject> ("floor", "floorMesh", "floorMaterial");
    floor.GetLocalTransform ().Scale (1000.0f, 0.1f, 1000.0f).Translate (0.0f, -0.05f, 0.0f);

    ControlledCamera& cam = gameScene.AddGameObject<ControlledCamera> ("camera", 60.0f, 10.0f, 600.0f, 5.0f, 0.1f);
    cam.Initialize ();
    cam.GetLocalTransform ().Translate (0.0f, 100.0f, 250.0f);
    gameScene.SetActiveCamera ("camera");

    PointLight& light = gameScene.AddGameObject<PointLight> ("light");
    light.GetLocalTransform ().Translate (100.0f, 100.0f, 100.0f);
    light.SetColor (Vector3f (1.0f, 1.0f, 1.0f));
    light.SetEnabled (true);

    DirectionalLight& light2 = gameScene.AddGameObject<DirectionalLight> ("light2");
    light2.GetLocalTransform ().Rotate (60.0f, 180.0f, 0.0f);
    light2.SetColor (Vector3f (1.0f, 1.0f, 1.0f));
    light2.SetEnabled (true);

    AnimationObject& anim = gameScene.GetGameObjectManager ().GetByName<AnimationObject> ("mixamo.com");
    anim.SetLooping (true);
    anim.Play();

    CGame::Run ();

    CGame::Deinitialize ();

    return 0;
}

