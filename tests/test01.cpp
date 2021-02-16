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
#include "resource/ResourceManager.h"
#include "resource/Texture.h"
#include "graphics/GLRenderer.h"
#include "graphics/GLFWRenderTarget.h"
#include "graphics/GLPostprocess.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"
#include "system/LogMessage.h"

#include "ControlledCamera.h"

#include <iostream>

int main (int argc, char* argv [])
{
    ResourceManager resourceManager;
    GameScene gameScene;
    GLFWRenderTarget renderTarget ("Test 1", 800, 600, false, true, true);
    GLRenderer renderer (800, 600);
    GLFWInputController inputController;

    Game* game = new Game (resourceManager, gameScene, renderer, renderTarget, inputController);

    renderer.AddPostprocess (new GLPostprocess (renderer, renderer.GetShaderProgram ("post_hdr_shader")));

    GLPostprocess* gamma = new GLPostprocess (renderer, renderer.GetShaderProgram("post_gamma_shader"));
    gamma->SetPostprocessParameterFloat ("fGamma", 2.0f);
    renderer.AddPostprocess (gamma);

    inputController.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    inputController.BindInputEvent ("exit", [ & ]() { game->Stop (); });

    inputController.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
    inputController.BindInputEvent ("mousemode", [ & ]() { inputController.SetMouseGameMode (!inputController.IsGameMode ()); });

    resourceManager.Load<Texture> ("tAlbedo", "textures/scuffed-metal1_albedo.png");
    resourceManager.Load<Texture> ("tMetalness", "textures/scuffed-metal1_metallic.png");
    resourceManager.Load<Texture> ("tRoughness", "textures/scuffed-metal1_roughness.png");
    resourceManager.Load<Texture> ("tAO", "textures/scuffed-metal1_ao.png");

    gameScene.AddMaterial<PBRMaterial> ("greenMaterial").SetAlbedo (Vector3f (0.1f, 0.4f, 0.1f)).SetRoughness (0.1f).SetMetallic (0.6f);
    gameScene.AddMaterial<PBRMaterial> ("redMaterial").SetAlbedo ("tAlbedo").SetMetallic ("tMetalness").SetRoughness ("tRoughness");
    gameScene.AddMaterial<PBRMaterial> ("goldMaterial").SetAlbedo (Vector3f (1.000f, 0.766f, 0.336f)).SetMetallic (0.8f).SetRoughness (0.2f).SetAO (1.0f);
    gameScene.AddMaterial<PBRMaterial> ("blueMaterial").SetAlbedo (Vector3f (0.02f, 0.29f, 0.53f)).SetMetallic (0.0f).SetRoughness(0.8f);
    gameScene.AddMaterial<PhongMaterial> ("lampMaterial").SetEmissive (Vector3f (0.9f, 0.9f, 0.9f)).SetDiffuse (Vector3f (0.2f, 0.2f, 0.2f));

    ControlledCamera& cam = gameScene.AddGameObject<ControlledCamera> ("camera", 60.0f, 0.01f, 100.0f, 0.1f);
    cam.Initialize ();
    cam.GetModelTransform ().Translate (5.0f, 2.5f, 5.0f).Rotate (-20.0f, 45.0f, 0.0f);
    gameScene.SetActiveCamera (&cam);

    MeshObject& cube = gameScene.AddGameObject<MeshObject> ("cube");
    Primitives::GenerateCube (cube);
    cube.SetMaterial ("redMaterial");
    cube.GetModelTransform ().Scale (0.5f).Translate (0.0f, 1.1f, 0.0f);

    MeshObject& cone = gameScene.AddGameObject<MeshObject> ("cone");
    Primitives::GenerateCone (cone, 16);
    cone.SetMaterial ("goldMaterial").SetSmoothNormals (false);
    cone.GetModelTransform ().Translate (-1.5f, 0.5f, 1.0f).Scale (0.5f);

    MeshObject& cylinder = gameScene.AddGameObject<MeshObject> ("cylinder");
    Primitives::GenerateCylinder (cylinder, 16);
    cylinder.SetMaterial ("blueMaterial").SetSmoothNormals (false);
    cylinder.GetModelTransform ().Rotate (90.0f, 12.5f, 0.0f).Translate (1.7f, 0.5f, 0.7f).Scale (0.5f);

    MeshObject& lamp = gameScene.AddGameObject<MeshObject> ("lamp");
    Primitives::GenerateSphere (lamp, 3);
    lamp.GetModelTransform ().Scale (0.1f, 0.1f, 0.1f).Translate (1.0f, 0.75f, 1.0f);
    lamp.SetMaterial ("lampMaterial");

    MeshObject& floor = gameScene.AddGameObject<MeshObject> ("floor");
    Primitives::GenerateCube (floor);
    floor.GetModelTransform ().Scale (2.5f, 0.05f, 2.5f).Translate (0.0f, -0.05f, 0.0f);
    floor.SetMaterial ("greenMaterial");

    PointLight& light1 = gameScene.AddGameObject<PointLight> ("light1");
    light1.SetParentObject (lamp);
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

    game->Run ();

    delete game;

    return 0;
}

