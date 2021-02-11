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
    gamma->SetPostprocessParameterFloat ("fGamma", 2.2f);
    renderer.AddPostprocess (gamma);

    inputController.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    inputController.BindInputEvent ("exit", [ & ]() { game->Stop (); });

    inputController.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
    inputController.BindInputEvent ("mousemode", [ & ]() { inputController.SetMouseGameMode (!inputController.IsGameMode ()); });

    unsigned int tAlbedo = resourceManager.Load<Texture> ("albedo", "textures/scuffed-metal1_albedo.png");
    unsigned int tMetalness = resourceManager.Load<Texture> ("metalness", "textures/scuffed-metal1_metallic.png");
    unsigned int tRoughness = resourceManager.Load<Texture> ("roughness", "textures/scuffed-metal1_roughness.png");
    unsigned int tAO = resourceManager.Load<Texture> ("ao", "textures/scuffed-metal1_ao.png");

    gameScene.AddMaterial<PBRMaterial> ("greenMaterial");
    gameScene.GetMaterials ().GetByName<PBRMaterial> ("greenMaterial").SetAlbedo (Vector3f (0.1f, 0.4f, 0.1f)).SetRoughness (0.1f).SetMetallic (0.6f);

    gameScene.AddMaterial<PBRMaterial> ("redMaterial");
    gameScene.GetMaterials ().GetByName<PBRMaterial> ("redMaterial").SetAlbedo (tAlbedo).SetMetallic (tMetalness).SetRoughness (tRoughness);

    gameScene.AddMaterial<PBRMaterial> ("goldMaterial");
    PBRMaterial& gold = gameScene.GetMaterials ().GetByName<PBRMaterial> ("goldMaterial");
    gold.SetAlbedo (Vector3f (1.000f, 0.766f, 0.336f));
    gold.SetMetallic (0.8f);
    gold.SetRoughness (0.2f);
    gold.SetAO (1.0f);

    gameScene.AddMaterial<PBRMaterial> ("blueMaterial");
    gameScene.GetMaterials ().GetByName<PBRMaterial> ("blueMaterial").SetAlbedo (Vector3f (0.02f, 0.29f, 0.53f)).SetMetallic (0.0f).SetRoughness(0.8f);

    gameScene.AddMaterial<PhongMaterial> ("lampMaterial");
    gameScene.GetMaterials ().GetByName<PhongMaterial> ("lampMaterial").SetEmissive (Vector3f (0.9f, 0.9f, 0.9f)).SetDiffuse (Vector3f (0.2f, 0.2f, 0.2f));

    ControlledCamera& cam = dynamic_cast<ControlledCamera&>(gameScene.AddGameObject (new ControlledCamera (60.0f, 0.01f, 100.0f, 0.1f)));
    cam.Initialize ();
    cam.GetModelTransform ().Translate (5.0f, 2.5f, 5.0f).Rotate (-20.0f, 45.0f, 0.0f);
    gameScene.SetActiveCamera (&cam);

    MeshObject& cube = dynamic_cast<MeshObject&>(gameScene.AddGameObject (new MeshObject ()));
    Primitives::GenerateCube (cube);
    cube.SetMaterial (gameScene.GetMaterials ().GetByName<PBRMaterial> ("redMaterial"));
    cube.GetModelTransform ().Scale (0.5f).Translate (0.0f, 1.1f, 0.0f);

    MeshObject& cone = dynamic_cast<MeshObject&>(gameScene.AddGameObject (new MeshObject ()));
    Primitives::GenerateCone (cone, 16);
    cone.SetMaterial (gold).SetSmoothNormals (false);
    cone.GetModelTransform ().Translate (-1.5f, 0.5f, 1.0f).Scale (0.5f);

    MeshObject& cylinder = dynamic_cast<MeshObject&>(gameScene.AddGameObject (new MeshObject ()));
    Primitives::GenerateCylinder (cylinder, 16);
    cylinder.SetMaterial (gameScene.GetMaterials ().GetByName<PBRMaterial> ("blueMaterial")).SetSmoothNormals (false);
    cylinder.GetModelTransform ().Rotate (90.0f, 12.5f, 0.0f).Translate (1.7f, 0.5f, 0.7f).Scale (0.5f);

    MeshObject& lamp = dynamic_cast<MeshObject&>(gameScene.AddGameObject (new MeshObject ()));
    Primitives::GenerateSphere (lamp, 3);
    lamp.GetModelTransform ().Scale (0.1f, 0.1f, 0.1f).Translate (1.0f, 0.75f, 1.0f);
    lamp.SetMaterial (gameScene.GetMaterials ().GetByName<PhongMaterial> ("lampMaterial"));

    MeshObject& floor = dynamic_cast<MeshObject&>(gameScene.AddGameObject (new MeshObject ()));
    Primitives::GenerateCube (floor);
    floor.GetModelTransform ().Scale (2.5f, 0.05f, 2.5f).Translate (0.0f, -0.05f, 0.0f);
    floor.SetMaterial (gameScene.GetMaterials ().GetByName<PBRMaterial> ("greenMaterial"));

    PointLight& light1 = dynamic_cast<PointLight&>(gameScene.AddGameObject (new PointLight ()));
    light1.SetParentObject (lamp);
    light1.SetColor (Vector3f (1.5f, 1.5f, 1.5f));
    light1.SetLinearAttenuationFactor (0.0f).SetQuadraticAttenuationFactor (1.0f);
    light1.SetEnabled (true);

    DirectionalLight& light2 = dynamic_cast<DirectionalLight&>(gameScene.AddGameObject (new DirectionalLight ()));
    light2.GetModelTransform ().Rotate (135.0f, 45.0f, 0.0f);
    light2.SetColor (Vector3f (2.7f, 2.7f, 2.7f));
    light2.SetEnabled (true);

    SpotLight& light3 = dynamic_cast<SpotLight&>(gameScene.AddGameObject (new SpotLight ()));
    light3.GetModelTransform ().Translate (2.0f, 10.0f, 0.0f).Rotate (90.0f, 0.0f, 0.0f);
    light3.SetColor (Vector3f (2.7f, 2.7f, 2.7f));
    light3.SetInnerCutoff (5.0f);
    light3.SetOuterCutoff (12.0f);
    light3.SetEnabled (true);

    SplinePath& lp = dynamic_cast<SplinePath&> (gameScene.AddGameObject (new SplinePath ()));
    lp.AddWaypoint ({2.0f, 0.0f, 2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));
    lp.AddWaypoint ({-2.0f, 0.0f, 2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));
    lp.AddWaypoint ({2.0f, 0.0f, -2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));
    lp.AddWaypoint ({2.0f, 0.0f, 2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));
    lp.AddWaypoint (2, {-2.0f, 2.0f, -2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));

    lp.SetStartTangent ({-2.0f, 0.0f, 2.0f});
    lp.SetEndTangent ({-2.0f, 0.0f, 2.0f});

    lp.GetModelTransform ().Rotate ({0.0f, 0.0f, -15.0f}).Translate ({0.0f, 1.0f, 0.0f});

    AnimationObject& lightAnimation = dynamic_cast<AnimationObject&> (gameScene.AddGameObject (new AnimationObject ()));

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

