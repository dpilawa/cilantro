#include "cilantroengine.h"
#include "game/Game.h"
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
#include "graphics/GLRenderer.h"
#include "graphics/GLFWRenderTarget.h"
#include "graphics/GLPostprocess.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"
#include "system/LogMessage.h"
#include "system/EngineContext.h"
#include "system/Timer.h"

#include "ControlledCamera.h"

#include <iostream>

int main (int argc, char* argv [])
{
    ResourceManager resourceManager;
    GameScene gameScene;
    GLFWRenderTarget renderTarget ("Test 1", 800, 600, false, true, true);
    GLRenderer renderer (800, 600);
    GLFWInputController inputController;
    Timer timer;
    Game game;

    EngineContext::Set (game, resourceManager, timer, gameScene, renderer, renderTarget, inputController);
    EngineContext::Initialize ();

    renderer.AddPostprocess<GLPostprocess> ("hdr_postprocess").SetShaderProgram ("post_hdr_shader");
    renderer.AddPostprocess<GLPostprocess> ("gamma_postprocess").SetShaderProgram ("post_gamma_shader").SetPostprocessParameterFloat ("fGamma", 2.1f);

    inputController.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    inputController.BindInputEvent ("exit", [ & ]() { game.Stop (); });

    inputController.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
    inputController.BindInputEvent ("mousemode", [ & ]() { inputController.SetMouseGameMode (!inputController.IsGameMode ()); });

    resourceManager.Load<Texture> ("tAlbedoMetal", "textures/scuffed-metal1_albedo.png");
    resourceManager.Load<Texture> ("tMetalnessMetal", "textures/scuffed-metal1_metallic.png");
    resourceManager.Load<Texture> ("tNormalMetal", "textures/scuffed-metal1_normal-dx.png");
    resourceManager.Load<Texture> ("tRoughnessMetal", "textures/scuffed-metal1_roughness.png");
    resourceManager.Load<Texture> ("tAOMetal", "textures/scuffed-metal1_ao.png");

    resourceManager.Load<Texture> ("tAlbedoGold", "textures/Metal007_1K_Color.png");
    resourceManager.Load<Texture> ("tMetalnessGold", "textures/Metal007_1K_Metalness.png");
    resourceManager.Load<Texture> ("tNormalGold", "textures/Metal007_1K_Normal.png");
    resourceManager.Load<Texture> ("tRoughnessGold", "textures/Metal007_1K_Roughness.png");

    gameScene.AddMaterial<PBRMaterial> ("greenMaterial").SetAlbedo (Vector3f (0.1f, 0.4f, 0.1f)).SetRoughness (0.1f).SetMetallic (0.6f).SetNormal ("tNormalMetal");
    gameScene.AddMaterial<PBRMaterial> ("redMaterial").SetAlbedo ("tAlbedoMetal").SetMetallic ("tMetalnessMetal").SetRoughness ("tRoughnessMetal").SetNormal("tNormalMetal").SetAO ("tAOMetal");
    gameScene.AddMaterial<PBRMaterial> ("goldMaterial").SetAlbedo ("tAlbedoGold").SetMetallic ("tMetalnessGold").SetRoughness ("tRoughnessGold").SetNormal("tNormalGold");
    gameScene.AddMaterial<PBRMaterial> ("blueMaterial").SetAlbedo (Vector3f (0.02f, 0.29f, 0.53f)).SetMetallic (0.0f).SetRoughness (0.8f);
    gameScene.AddMaterial<PhongMaterial> ("lampMaterial").SetEmissive (Vector3f (0.9f, 0.9f, 0.9f)).SetDiffuse (Vector3f (0.2f, 0.2f, 0.2f));

    ControlledCamera& cam = gameScene.AddGameObject<ControlledCamera> ("camera", 60.0f, 0.01f, 100.0f, 0.1f);
    cam.Initialize ();
    cam.GetModelTransform ().Translate (5.0f, 2.5f, 5.0f).Rotate (-20.0f, 45.0f, 0.0f);
    gameScene.SetActiveCamera ("camera");

    Mesh& cubeMesh = resourceManager.Create<Mesh> ("cubeMesh");
    Primitives::GenerateCube (cubeMesh);
    MeshObject& cube = gameScene.AddGameObject<MeshObject> ("cube", "cubeMesh", "redMaterial");
    cube.GetModelTransform ().Scale (0.5f).Translate (0.0f, 1.1f, 0.0f);

    Mesh& coneMesh = resourceManager.Create<Mesh> ("coneMesh").SetSmoothNormals (false);
    Primitives::GenerateCone (coneMesh, 32);
    MeshObject& cone = gameScene.AddGameObject<MeshObject> ("cone", "coneMesh", "goldMaterial");
    cone.GetModelTransform ().Translate (-1.5f, 0.5f, 1.0f).Scale (0.5f);

    Mesh& cylinderMesh = resourceManager.Create<Mesh> ("cylinderMesh").SetSmoothNormals (false);
    Primitives::GenerateCylinder (cylinderMesh, 32);
    MeshObject& cylinder = gameScene.AddGameObject<MeshObject> ("cylinder", "cylinderMesh", "blueMaterial");
    cylinder.GetModelTransform ().Rotate (90.0f, 12.5f, 0.0f).Translate (1.7f, 0.5f, 0.7f).Scale (0.5f);

    Mesh& lampMesh = resourceManager.Create<Mesh> ("lampMesh");
    Primitives::GenerateSphere (lampMesh, 3);
    MeshObject& lamp = gameScene.AddGameObject<MeshObject> ("lamp", "lampMesh", "lampMaterial");
    lamp.GetModelTransform ().Scale (0.1f, 0.1f, 0.1f).Translate (1.0f, 0.75f, 1.0f);

    Mesh& floorMesh = resourceManager.Create<Mesh> ("floorMesh");
    Primitives::GenerateCube (floorMesh);
    MeshObject& floor = gameScene.AddGameObject<MeshObject> ("floor", "floorMesh", "greenMaterial");
    floor.GetModelTransform ().Scale (2.5f, 0.05f, 2.5f).Translate (0.0f, -0.05f, 0.0f);

    PointLight& light1 = gameScene.AddGameObject<PointLight> ("light1");
    light1.SetParentObject ("lamp");
    light1.SetColor (Vector3f (1.5f, 1.5f, 1.5f));
    light1.SetLinearAttenuationFactor (0.0f).SetQuadraticAttenuationFactor (1.0f);
    light1.SetEnabled (true);

    DirectionalLight& light2 = gameScene.AddGameObject<DirectionalLight> ("light2");
    light2.GetModelTransform ().Rotate (135.0f, 45.0f, 0.0f);
    light2.SetColor (Vector3f (2.7f, 2.7f, 2.7f));
    light2.SetEnabled (true);

    SpotLight& light3 = gameScene.AddGameObject<SpotLight> ("light3");
    light3.GetModelTransform ().Translate (2.0f, 10.0f, 0.0f).Rotate (90.0f, 0.0f, 0.0f);
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

    lp.GetModelTransform ().Rotate ({0.0f, 0.0f, -15.0f}).Translate ({0.0f, 1.0f, 0.0f});

    AnimationObject& lightAnimation = gameScene.AddGameObject<AnimationObject> ("lightAnimation");

    lightAnimation.AddAnimationProperty<float> (
        "t", 0.0f,
        [&] (float t) {
            lamp.GetModelTransform ().Translate (lp.GetPositionAtDistance (lp.GetPathLength () * t));
            lamp.GetModelTransform ().Rotate (lp.GetRotationAtDistance (lp.GetPathLength () * t));
        },
        [] (float t0, float t1, float u) { return Mathf::Lerp (t0, t1, u); });

    lightAnimation.AddKeyframe<float> ("t", 5.0f, 1.0f);
    lightAnimation.SetLooping (true);
    lightAnimation.Play ();

    game.Run ();

    EngineContext::Deinitialize ();

    return 0;
}

