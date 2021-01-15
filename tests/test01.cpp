#include "cilantroengine.h"
#include "game/GameLoop.h"
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
    GameLoop game;

    GameScene scene (&game);
    game.gameScene = &scene;
    
    GLFWRenderTarget target (&game, "Test 1", 800, 600, false, true, true);
    game.gameRenderTarget = dynamic_cast<RenderTarget*>(&target);

    GLFWInputController controller (&game, target.GetWindow ());
    game.gameInputController = dynamic_cast<InputController*>(&controller);

    GLRenderer renderer (&game, 800, 600);
    game.gameRenderer = dynamic_cast<Renderer*>(&renderer);

    GLPostprocess hdr (&renderer, &renderer.GetShaderProgram("post_hdr_shader"));
    renderer.AddPostprocess (&hdr);

    GLPostprocess gamma (&renderer, &renderer.GetShaderProgram("post_gamma_shader"));
    gamma.SetPostprocessParameterFloat ("fGamma", 2.2f);
    renderer.AddPostprocess (&gamma);

    controller.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    controller.BindInputEvent ("exit", [ & ]() { game.Stop (); });

    controller.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
    controller.BindInputEvent ("mousemode", [ & ]() { controller.SetMouseGameMode (!controller.IsGameMode ()); });

    Texture albedo ("albedo", "textures/scuffed-metal1_albedo.png");
    Texture metalness ("metalness", "textures/scuffed-metal1_metallic.png");
    Texture roughness ("roughness", "textures/scuffed-metal1_roughness.png");
    Texture ao ("ao", "textures/scuffed-metal1_ao.png");

    PBRMaterial& green = dynamic_cast<PBRMaterial&>(scene.AddMaterial (new PBRMaterial ()));
    green.SetAlbedo (Vector3f (0.1f, 0.4f, 0.1f)).SetRoughness (0.1f).SetMetallic (0.6f);

    PBRMaterial& red = dynamic_cast<PBRMaterial&>(scene.AddMaterial (new PBRMaterial ()));
    //red.SetAlbedo (Vector3f (0.9f, 0.1f, 0.1f)).SetMetallic(0.2f).SetRoughness(0.4f);
    red.SetAlbedo (&albedo).SetMetallic (&metalness).SetRoughness (&roughness);

    PBRMaterial& gold = dynamic_cast<PBRMaterial&>(scene.AddMaterial (new PBRMaterial ()));
    gold.SetAlbedo (Vector3f (1.000f, 0.766f, 0.336f));
    gold.SetMetallic (0.8f);
    gold.SetRoughness (0.2f);
    gold.SetAO (1.0f);

    PBRMaterial& blue = dynamic_cast<PBRMaterial&>(scene.AddMaterial (new PBRMaterial ()));
    blue.SetAlbedo (Vector3f (0.02f, 0.29f, 0.53f)).SetMetallic (0.0f).SetRoughness(0.8f);

    PhongMaterial& lampM = dynamic_cast<PhongMaterial&>(scene.AddMaterial (new PhongMaterial ()));
    lampM.SetEmissive (Vector3f (0.9f, 0.9f, 0.9f)).SetDiffuse (Vector3f (0.2f, 0.2f, 0.2f));

    ControlledCamera& cam = dynamic_cast<ControlledCamera&>(scene.AddGameObject (new ControlledCamera (60.0f, 0.01f, 100.0f, 0.1f)));
    cam.Initialize ();
    cam.GetModelTransform ().Translate (5.0f, 2.5f, 5.0f).Rotate (-20.0f, 45.0f, 0.0f);
    scene.SetActiveCamera (&cam);

    MeshObject& cube = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    Primitives::GenerateCube (cube);
    cube.SetMaterial (red);
    cube.GetModelTransform ().Scale (0.5f).Translate (0.0f, 1.1f, 0.0f);

    MeshObject& cone = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    Primitives::GenerateCone (cone, 16);
    cone.SetMaterial (gold).SetSmoothNormals (false);
    cone.GetModelTransform ().Translate (-1.5f, 0.5f, 1.0f).Scale (0.5f);

    MeshObject& cylinder = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    Primitives::GenerateCylinder (cylinder, 16);
    cylinder.SetMaterial (blue).SetSmoothNormals (false);
    cylinder.GetModelTransform ().Rotate (90.0f, 12.5f, 0.0f).Translate (1.7f, 0.5f, 0.7f).Scale (0.5f);

    MeshObject& lamp = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    Primitives::GenerateSphere (lamp, 3);
    lamp.GetModelTransform ().Scale (0.1f, 0.1f, 0.1f).Translate (1.0f, 0.75f, 1.0f);
    lamp.SetMaterial (lampM);

    MeshObject& floor = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    Primitives::GenerateCube (floor);
    floor.GetModelTransform ().Scale (2.5f, 0.05f, 2.5f).Translate (0.0f, -0.05f, 0.0f);
    floor.SetMaterial (green);

    PointLight& light1 = dynamic_cast<PointLight&>(scene.AddGameObject (new PointLight ()));
    light1.SetParentObject (lamp);
    light1.SetColor (Vector3f (1.5f, 1.5f, 1.5f));
    light1.SetLinearAttenuationFactor (0.0f).SetQuadraticAttenuationFactor (1.0f);
    light1.SetEnabled (true);

    DirectionalLight& light2 = dynamic_cast<DirectionalLight&>(scene.AddGameObject (new DirectionalLight ()));
    light2.GetModelTransform ().Rotate (135.0f, 45.0f, 0.0f);
    light2.SetColor (Vector3f (2.7f, 2.7f, 2.7f));
    light2.SetEnabled (true);

    SpotLight& light3 = dynamic_cast<SpotLight&>(scene.AddGameObject (new SpotLight ()));
    light3.GetModelTransform ().Translate (2.0f, 10.0f, 0.0f).Rotate (90.0f, 0.0f, 0.0f);
    light3.SetColor (Vector3f (2.7f, 2.7f, 2.7f));
    light3.SetInnerCutoff (5.0f);
    light3.SetOuterCutoff (12.0f);
    light3.SetEnabled (true);

    SplinePath& lp = dynamic_cast<SplinePath&> (scene.AddGameObject (new SplinePath ()));
    lp.AddWaypoint ({2.0f, 0.0f, 2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));
    lp.AddWaypoint ({-2.0f, 0.0f, 2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));
    lp.AddWaypoint ({2.0f, 0.0f, -2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));
    lp.AddWaypoint ({2.0f, 0.0f, 2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));
    lp.AddWaypoint (2, {-2.0f, 2.0f, -2.0f}, Mathf::EulerToQuaterion ({0.0f, 0.0f, 0.0f}));

    lp.SetStartTangent ({-2.0f, 0.0f, 2.0f});
    lp.SetEndTangent ({-2.0f, 0.0f, 2.0f});

    lp.GetModelTransform ().Rotate ({0.0f, 0.0f, -15.0f}).Translate ({0.0f, 1.0f, 0.0f});

    AnimationObject& lightAnimation = dynamic_cast<AnimationObject&> (scene.AddGameObject (new AnimationObject ()));

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

    return 0;
}

