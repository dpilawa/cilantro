#include "cilantroengine.h"
#include "scene/Primitives.h"
#include "scene/AnimationObject.h"
#include "scene/GameScene.h"
#include "scene/PhongMaterial.h"
#include "scene/PBRMaterial.h"
#include "scene/MeshObject.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "scene/SpotLight.h"
#include "scene/LinearPath.h"
#include "scene/SplinePath.h"
#include "resource/Mesh.h"
#include "resource/ResourceManager.h"
#include "resource/Texture.h"
#include "graphics/QuadRenderStage.h"
#include "graphics/GLFWRenderer.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"
#include "system/LogMessage.h"
#include "system/Game.h"
#include "system/Timer.h"

#include "ControlledCamera.h"

#include <iostream>

int main (int argc, char* argv [])
{
    CGame::Initialize (); 

    CGameScene& gameScene = CGame::CreateGameScene<CGameScene> ("scene");
    CGLFWRenderer& renderer = gameScene.CreateRenderer<CGLFWRenderer> (800, 600, true, false, "Test 01", false, true, true);
    InputController& inputController = CGame::CreateInputController<GLFWInputController> ();

    renderer.AddRenderStage<CQuadRenderStage> ("hdr_postprocess").SetShaderProgram ("post_hdr_shader").SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS);
    renderer.AddRenderStage<CQuadRenderStage> ("fxaa_postprocess").SetShaderProgram ("post_fxaa_shader").SetRenderStageParameterFloat ("fMaxSpan", 4.0f).SetRenderStageParameterVector2f ("vInvResolution", Vector2f (1.0f / renderer.GetWidth (), 1.0f / renderer.GetHeight ())).SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS);
    renderer.AddRenderStage<CQuadRenderStage> ("gamma_postprocess+screen").SetShaderProgram ("post_gamma_shader").SetRenderStageParameterFloat ("fGamma", 2.1f).SetColorAttachmentsFramebufferLink (EPipelineLink::LINK_PREVIOUS).SetFramebufferEnabled (false);

    inputController.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    inputController.BindInputEvent ("exit", [ & ]() { CGame::Stop (); });

    inputController.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
    inputController.BindInputEvent ("mousemode", [ & ]() { inputController.SetMouseGameMode (!inputController.IsGameMode ()); });

    CGame::GetResourceManager ().Load<Texture> ("tAlbedoMetal", "textures/scuffed-metal1_albedo.png");
    CGame::GetResourceManager ().Load<Texture> ("tMetalnessMetal", "textures/scuffed-metal1_metallic.png");
    CGame::GetResourceManager ().Load<Texture> ("tNormalMetal", "textures/scuffed-metal1_normal-dx.png");
    CGame::GetResourceManager ().Load<Texture> ("tRoughnessMetal", "textures/scuffed-metal1_roughness.png");
    CGame::GetResourceManager ().Load<Texture> ("tAOMetal", "textures/scuffed-metal1_ao.png");

    CGame::GetResourceManager ().Load<Texture> ("tAlbedoGold", "textures/Metal007_1K_Color.png");
    CGame::GetResourceManager ().Load<Texture> ("tMetalnessGold", "textures/Metal007_1K_Metalness.png");
    CGame::GetResourceManager ().Load<Texture> ("tNormalGold", "textures/Metal007_1K_Normal.png");
    CGame::GetResourceManager ().Load<Texture> ("tRoughnessGold", "textures/Metal007_1K_Roughness.png");

    gameScene.AddMaterial<PBRMaterial> ("greenMaterial").SetAlbedo (Vector3f (0.1f, 0.4f, 0.1f)).SetRoughness (0.1f).SetMetallic (0.6f).SetNormal ("tNormalMetal");
    gameScene.AddMaterial<PBRMaterial> ("redMaterial").SetAlbedo ("tAlbedoMetal").SetMetallic ("tMetalnessMetal").SetRoughness ("tRoughnessMetal").SetNormal("tNormalMetal").SetAO ("tAOMetal");
    gameScene.AddMaterial<PBRMaterial> ("goldMaterial").SetAlbedo ("tAlbedoGold").SetMetallic ("tMetalnessGold").SetRoughness ("tRoughnessGold").SetNormal("tNormalGold");
    gameScene.AddMaterial<PBRMaterial> ("blueMaterial").SetAlbedo (Vector3f (0.02f, 0.29f, 0.53f)).SetMetallic (0.0f).SetRoughness (0.8f);
    gameScene.AddMaterial<PhongMaterial> ("lampMaterial").SetEmissive (Vector3f (0.9f, 0.9f, 0.9f)).SetDiffuse (Vector3f (0.2f, 0.2f, 0.2f));

    ControlledCamera& cam = gameScene.AddGameObject<ControlledCamera> ("camera", 60.0f, 0.5f, 10.0f, 0.1f, 0.1f);
    cam.Initialize ();
    cam.GetLocalTransform ().Translate (5.0f, 2.5f, 5.0f).Rotate (-20.0f, 45.0f, 0.0f);
    gameScene.SetActiveCamera ("camera");

    Mesh& cubeMesh = CGame::GetResourceManager ().Create<Mesh> ("cubeMesh");
    Primitives::GenerateCube (cubeMesh);
    MeshObject& cube = gameScene.AddGameObject<MeshObject> ("cube", "cubeMesh", "redMaterial");
    cube.GetLocalTransform ().Scale (0.5f).Translate (0.0f, 1.1f, 0.0f);

    Mesh& coneMesh = CGame::GetResourceManager ().Create<Mesh> ("coneMesh").SetSmoothNormals (false);
    Primitives::GenerateCone (coneMesh, 32);
    MeshObject& cone = gameScene.AddGameObject<MeshObject> ("cone", "coneMesh", "goldMaterial");
    cone.GetLocalTransform ().Translate (-1.5f, 0.5f, 1.0f).Scale (0.5f);

    Mesh& cylinderMesh = CGame::GetResourceManager ().Create<Mesh> ("cylinderMesh").SetSmoothNormals (false);
    Primitives::GenerateCylinder (cylinderMesh, 32);
    MeshObject& cylinder = gameScene.AddGameObject<MeshObject> ("cylinder", "cylinderMesh", "blueMaterial");
    cylinder.GetLocalTransform ().Rotate (90.0f, 12.5f, 0.0f).Translate (1.7f, 0.5f, 0.7f).Scale (0.5f);

    Mesh& lampMesh = CGame::GetResourceManager ().Create<Mesh> ("lampMesh");
    Primitives::GenerateSphere (lampMesh, 3);
    MeshObject& lamp = gameScene.AddGameObject<MeshObject> ("lamp", "lampMesh", "lampMaterial");
    lamp.GetLocalTransform ().Scale (0.1f, 0.1f, 0.1f).Translate (1.0f, 0.75f, 1.0f);

    Mesh& floorMesh = CGame::GetResourceManager ().Create<Mesh> ("floorMesh");
    Primitives::GenerateCube (floorMesh);
    MeshObject& floor = gameScene.AddGameObject<MeshObject> ("floor", "floorMesh", "greenMaterial");
    floor.GetLocalTransform ().Scale (2.5f, 0.05f, 2.5f).Translate (0.0f, -0.05f, 0.0f);

    PointLight& light1 = gameScene.AddGameObject<PointLight> ("light1");
    light1.SetParentObject ("lamp");
    light1.SetColor (Vector3f (1.5f, 1.5f, 1.5f));
    light1.SetLinearAttenuationFactor (0.0f).SetQuadraticAttenuationFactor (1.0f);
    light1.SetEnabled (true);

    DirectionalLight& light2 = gameScene.AddGameObject<DirectionalLight> ("light2");
    light2.GetLocalTransform ().Rotate (45.0f, -135.0f, 0.0f);
    light2.SetColor (Vector3f (2.7f, 2.7f, 2.7f));
    light2.SetEnabled (true);

    SpotLight& light3 = gameScene.AddGameObject<SpotLight> ("light3");
    light3.GetLocalTransform ().Translate (2.0f, 10.0f, 0.0f).Rotate (90.0f, 0.0f, 0.0f);
    light3.SetColor (Vector3f (2.7f, 2.7f, 2.7f));
    light3.SetInnerCutoff (5.0f);
    light3.SetOuterCutoff (12.0f);
    light3.SetEnabled (true);

    SplinePath& lp = gameScene.AddGameObject<SplinePath> ("splinepath");
    lp.AddWaypoint ({2.0f, 0.0f, 2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));
    lp.AddWaypoint ({-2.0f, 0.0f, 2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));
    lp.AddWaypoint ({2.0f, 0.0f, -2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));
    lp.AddWaypoint ({2.0f, 0.0f, 2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));
    lp.AddWaypoint (2, {-2.0f, 2.0f, -2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));

    lp.SetStartTangent ({-2.0f, 0.0f, 2.0f});
    lp.SetEndTangent ({-2.0f, 0.0f, 2.0f});

    lp.GetLocalTransform ().Rotate ({0.0f, 0.0f, -15.0f}).Translate ({0.0f, 1.0f, 0.0f});

    AnimationObject& lightAnimation = gameScene.AddGameObject<AnimationObject> ("lightAnimation");

    lightAnimation.AddAnimationProperty<float> (
        "t", 0.0f,
        [&] (float t) {
            lamp.GetLocalTransform ().Translate (lp.GetPositionAtDistance (lp.GetPathLength () * t));
            lamp.GetLocalTransform ().Rotate (lp.GetRotationAtDistance (lp.GetPathLength () * t));
        },
        [] (float t0, float t1, float u) { return Mathf::Lerp (t0, t1, u); });

    lightAnimation.AddKeyframe<float> ("t", 5.0f, 1.0f);
    lightAnimation.SetLooping (true);
    lightAnimation.Play ();

    CGame::Run ();

    CGame::Deinitialize ();

    return 0;
}

