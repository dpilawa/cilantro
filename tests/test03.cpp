#include "cilantroengine.h"
#include "game/GameLoop.h"
#include "scene/Primitives.h"
#include "scene/AnimationObject.h"
#include "scene/GameScene.h"
#include "scene/PhongMaterial.h"
#include "scene/MeshObject.h"
#include "scene/DirectionalLight.h"
#include "graphics/GLRenderer.h"
#include "graphics/GLFWRenderTarget.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"

#include "ControlledCamera.h"

#include <iostream>

int main (int argc, char* argv[])
{
    GameLoop game;

    GameScene scene (&game);
    game.gameScene = &scene;

    GLFWRenderTarget target (&game, "Test 3", 800, 600, false, true, true);
    game.gameRenderTarget = dynamic_cast<RenderTarget*>(&target);

    GLFWInputController controller (&game, target.GetWindow ());
    game.gameInputController = dynamic_cast<InputController*>(&controller);

    GLRenderer renderer (&game, 800, 600);
    game.gameRenderer = dynamic_cast<Renderer*>(&renderer);

    controller.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    controller.BindInputEvent ("exit", [ & ]() { game.Stop (); });

    controller.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
    controller.BindInputEvent ("mousemode", [ & ]() { controller.SetMouseGameMode (!controller.IsGameMode ()); });

    PhongMaterial& m = dynamic_cast<PhongMaterial&>(scene.AddMaterial (new PhongMaterial ()));
    m.SetShaderProgram ("blinnphong_shader");
    m.SetColor (Vector3f (0.4f, 0.4f, 0.4f));
    m.SetAmbientColor (Vector3f (1.0f, 1.0f, 1.0f));
    m.SetSpecularColor (Vector3f (1.0f, 1.0f, 1.0f)).SetSpecularShininess (64.0f);

    ControlledCamera& cam = dynamic_cast<ControlledCamera&>(scene.AddGameObject (new ControlledCamera (60.0f, 0.1f, 100.0f, 0.1f)));
    cam.Initialize ();
    cam.GetModelTransform ().Translate (15.0f, 15.0f, 15.0f).Rotate (-35.0f, 45.0f, 0.0f);
    scene.SetActiveCamera (&cam);

    GameObject& plane = scene.AddGameObject (new GameObject ());

    MeshObject& fuselage = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    Primitives::GenerateCube (fuselage, false);
    fuselage.GetModelTransform ().Scale (1.0f, 1.0f, 10.0f);
    fuselage.SetMaterial (m);
    fuselage.SetParentObject (plane);

    MeshObject& wings = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    Primitives::GenerateCube (wings, false);
    wings.GetModelTransform ().Scale (12.0f, 0.2f, 1.0f).Translate (0.0f, 0.0f, 2.0f);
    wings.SetMaterial (m);
    wings.SetParentObject (plane);

    MeshObject& rudder = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    Primitives::GenerateCube (rudder, false);
    rudder.GetModelTransform ().Scale (0.2f, 2.0f, 1.0f).Translate (0.0f, 1.5f, -5.0f).Rotate (-10.0f, 0.0f, 0.0f);
    rudder.SetMaterial (m);
    rudder.SetParentObject (plane);

    MeshObject& tail = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    Primitives::GenerateCube (tail, false);
    tail.GetModelTransform ().Scale (4.0f, 0.2f, 1.0f).Translate (0.0f, 0.2f, -5.0f);
    tail.SetMaterial (m);
    tail.SetParentObject (plane);

    DirectionalLight& light = dynamic_cast<DirectionalLight&>(scene.AddGameObject (new DirectionalLight ()));
    light.GetModelTransform ().Rotate (90.0f, 0.0f, 10.0f);
    light.SetColor (Vector3f (1.0f, 1.0f, 1.0f));
    light.SetSpecularPower (2.0f);
    light.SetAmbiencePower (0.3f);
    light.SetEnabled (true);

    AnimationObject& anim1 = dynamic_cast<AnimationObject&> (scene.AddGameObject (new AnimationObject ()));
    anim1.AddAnimationProperty<Quaternion> ("rotation", Mathf::EulerToQuaterion (Mathf::Deg2Rad (Vector3f (0.0f, 0.0f, 0.0f))), [&](Quaternion q) { plane.GetModelTransform ().Rotate (q); }, [](Quaternion q0, Quaternion q1, float t) { return Mathf::Slerp (q0, q1, t); });
    anim1.AddKeyframe<Quaternion> ("rotation", 2.0f, Mathf::EulerToQuaterion (Mathf::Deg2Rad (Vector3f (-35.0f, 0.0f, 0.0f))));
    anim1.AddKeyframe<Quaternion> ("rotation", 5.0f, Mathf::EulerToQuaterion (Mathf::Deg2Rad (Vector3f (-35.0f, 20.0f, -45.0f))));
    anim1.AddKeyframe<Quaternion> ("rotation", 11.0f, Mathf::EulerToQuaterion (Mathf::Deg2Rad (Vector3f (-35.0f, -20.0f, 45.0f))));
    anim1.AddKeyframe<Quaternion> ("rotation", 14.0f, Mathf::EulerToQuaterion (Mathf::Deg2Rad (Vector3f (-0.0f, 0.0f, 0.0f))));
    anim1.SetLooping (true);
    anim1.Play ();

    game.Run ();

    return 0;
}

