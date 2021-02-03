#include "cilantroengine.h"
#include "game/Game.h"
#include "scene/Primitives.h"
#include "scene/AnimationObject.h"
#include "scene/GameScene.h"
#include "scene/PhongMaterial.h"
#include "scene/MeshObject.h"
#include "scene/DirectionalLight.h"
#include "resource/ResourceManager.h"
#include "graphics/GLRenderer.h"
#include "graphics/GLFWRenderTarget.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"

#include "ControlledCamera.h"

#include <iostream>

int main (int argc, char* argv[])
{
    ResourceManager resourceManager;
    GameScene gameScene;
    GLFWRenderTarget renderTarget ("Test 3", 800, 600, false, true, true);
    GLRenderer renderer (800, 600);
    GLFWInputController inputController;

    Game* game = new Game (resourceManager, gameScene, renderer, renderTarget, inputController);

    inputController.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    inputController.BindInputEvent ("exit", [ & ]() { game->Stop (); });

    inputController.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
    inputController.BindInputEvent ("mousemode", [ & ]() { inputController.SetMouseGameMode (!inputController.IsGameMode ()); });

    PhongMaterial& m = dynamic_cast<PhongMaterial&>(gameScene.AddMaterial (new PhongMaterial ()));
    m.SetShaderProgram ("blinnphong_shader");
    m.SetDiffuse (Vector3f (0.4f, 0.4f, 0.4f));
    m.SetSpecular (Vector3f (1.0f, 1.0f, 1.0f)).SetSpecularShininess (64.0f);

    ControlledCamera& cam = dynamic_cast<ControlledCamera&>(gameScene.AddGameObject (new ControlledCamera (60.0f, 0.1f, 100.0f, 0.1f)));
    cam.Initialize ();
    cam.GetModelTransform ().Translate (15.0f, 15.0f, 15.0f).Rotate (-35.0f, 45.0f, 0.0f);
    gameScene.SetActiveCamera (&cam);

    GameObject& plane = gameScene.AddGameObject (new GameObject ());

    MeshObject& fuselage = dynamic_cast<MeshObject&>(gameScene.AddGameObject (new MeshObject ()));
    Primitives::GenerateCube (fuselage);
    fuselage.GetModelTransform ().Scale (1.0f, 1.0f, 10.0f);
    fuselage.SetMaterial (m);
    fuselage.SetParentObject (plane);

    MeshObject& wings = dynamic_cast<MeshObject&>(gameScene.AddGameObject (new MeshObject ()));
    Primitives::GenerateCube (wings);
    wings.GetModelTransform ().Scale (12.0f, 0.2f, 1.0f).Translate (0.0f, 0.0f, 2.0f);
    wings.SetMaterial (m);
    wings.SetParentObject (plane);

    MeshObject& rudder = dynamic_cast<MeshObject&>(gameScene.AddGameObject (new MeshObject ()));
    Primitives::GenerateCube (rudder);
    rudder.GetModelTransform ().Scale (0.2f, 2.0f, 1.0f).Translate (0.0f, 1.5f, -5.0f).Rotate (-10.0f, 0.0f, 0.0f);
    rudder.SetMaterial (m);
    rudder.SetParentObject (plane);

    MeshObject& tail = dynamic_cast<MeshObject&>(gameScene.AddGameObject (new MeshObject ()));
    Primitives::GenerateCube (tail);
    tail.GetModelTransform ().Scale (4.0f, 0.2f, 1.0f).Translate (0.0f, 0.2f, -5.0f);
    tail.SetMaterial (m);
    tail.SetParentObject (plane);

    DirectionalLight& light = dynamic_cast<DirectionalLight&>(gameScene.AddGameObject (new DirectionalLight ()));
    light.GetModelTransform ().Rotate (90.0f, 0.0f, 10.0f);
    light.SetColor (Vector3f (1.0f, 1.0f, 1.0f));
    light.SetEnabled (true);

    AnimationObject& anim1 = dynamic_cast<AnimationObject&> (gameScene.AddGameObject (new AnimationObject ()));
    anim1.AddAnimationProperty<Quaternion> ("rotation", Mathf::EulerToQuaterion (Mathf::Deg2Rad (Vector3f (0.0f, 0.0f, 0.0f))), [&](Quaternion q) { plane.GetModelTransform ().Rotate (q); }, [](Quaternion q0, Quaternion q1, float t) { return Mathf::Slerp (q0, q1, Mathf::Smoothstep(0.0f, 1.0f, t)); });
    anim1.AddKeyframe<Quaternion> ("rotation", 2.0f, Mathf::EulerToQuaterion (Mathf::Deg2Rad (Vector3f (-35.0f, 0.0f, 0.0f))));
    anim1.AddKeyframe<Quaternion> ("rotation", 5.0f, Mathf::EulerToQuaterion (Mathf::Deg2Rad (Vector3f (-35.0f, 20.0f, -45.0f))));
    anim1.AddKeyframe<Quaternion> ("rotation", 11.0f, Mathf::EulerToQuaterion (Mathf::Deg2Rad (Vector3f (-35.0f, -20.0f, 45.0f))));
    anim1.AddKeyframe<Quaternion> ("rotation", 14.0f, Mathf::EulerToQuaterion (Mathf::Deg2Rad (Vector3f (-0.0f, 0.0f, 0.0f))));
    anim1.SetLooping (true);
    anim1.Play ();

    game->Run ();

    delete game;

    return 0;
}

