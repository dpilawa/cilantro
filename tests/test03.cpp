#include "cilantroengine.h"
#include "game/Game.h"
#include "scene/Primitives.h"
#include "scene/AnimationObject.h"
#include "scene/GameScene.h"
#include "scene/PhongMaterial.h"
#include "scene/MeshObject.h"
#include "scene/DirectionalLight.h"
#include "resource/ResourceManager.h"
#include "graphics/GLForwardRenderer.h"
#include "graphics/GLDeferredRenderer.h"
#include "graphics/GLFWRenderTarget.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"
#include "system/EngineContext.h"
#include "system/Timer.h"

#include "ControlledCamera.h"

#include <iostream>

int main (int argc, char* argv[])
{
    ResourceManager resourceManager;
    GameScene gameScene;
    GLFWRenderTarget renderTarget ("Test 3", 800, 600, false, true, true);
    GLDeferredRenderer renderer (800, 600);
    GLFWInputController inputController;
    Timer timer;
    Game game;

    EngineContext::Set (game, resourceManager, timer, gameScene, renderer, renderTarget, inputController);
    EngineContext::Initialize ();

    inputController.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
    inputController.BindInputEvent ("exit", [ & ]() { game.Stop (); });

    inputController.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
    inputController.BindInputEvent ("mousemode", [ & ]() { inputController.SetMouseGameMode (!inputController.IsGameMode ()); });

    PhongMaterial& m = gameScene.AddMaterial<PhongMaterial> ("material");
    m.SetDiffuse (Vector3f (0.4f, 0.4f, 0.4f)).SetSpecular (Vector3f (1.0f, 1.0f, 1.0f)).SetSpecularShininess (64.0f);

    ControlledCamera& cam = gameScene.AddGameObject<ControlledCamera> ("camera", 60.0f, 0.1f, 100.0f, 0.1f);
    cam.Initialize ();
    cam.GetModelTransform ().Translate (15.0f, 15.0f, 15.0f).Rotate (-35.0f, 45.0f, 0.0f);
    gameScene.SetActiveCamera ("camera");

    GameObject& plane = gameScene.AddGameObject<GameObject> ("plane");

    Mesh& fuselageMesh = resourceManager.Create<Mesh> ("fuselageMesh");
    MeshObject& fuselage = gameScene.AddGameObject<MeshObject> ("fuselage", "fuselageMesh", "material");
    Primitives::GenerateCube (fuselageMesh);
    fuselage.GetModelTransform ().Scale (1.0f, 1.0f, 10.0f);
    fuselage.SetParentObject ("plane");

    Mesh& wingsMesh = resourceManager.Create<Mesh> ("wingsMesh");
    MeshObject& wings = gameScene.AddGameObject<MeshObject> ("wings", "wingsMesh", "material");
    Primitives::GenerateCube (wingsMesh);
    wings.GetModelTransform ().Scale (12.0f, 0.2f, 1.0f).Translate (0.0f, 0.0f, 2.0f);
    wings.SetParentObject ("plane");

    Mesh& rudderMesh = resourceManager.Create<Mesh> ("rudderMesh");
    MeshObject& rudder = gameScene.AddGameObject<MeshObject> ("rudder", "rudderMesh", "material");
    Primitives::GenerateCube (rudderMesh);
    rudder.GetModelTransform ().Scale (0.2f, 2.0f, 1.0f).Translate (0.0f, 1.5f, -5.0f).Rotate (-10.0f, 0.0f, 0.0f);
    rudder.SetParentObject ("plane");

    Mesh& tailMesh = resourceManager.Create<Mesh> ("tailMesh");
    MeshObject& tail = gameScene.AddGameObject<MeshObject> ("tail", "tailMesh", "material");
    Primitives::GenerateCube (tailMesh);
    tail.GetModelTransform ().Scale (4.0f, 0.2f, 1.0f).Translate (0.0f, 0.2f, -5.0f);
    tail.SetParentObject ("plane");

    DirectionalLight& light = gameScene.AddGameObject<DirectionalLight> ("light");
    light.GetModelTransform ().Rotate (90.0f, 10.0f, 10.0f);
    light.SetColor (Vector3f (1.0f, 1.0f, 1.0f));
    light.SetEnabled (true);

    AnimationObject& anim1 = gameScene.AddGameObject<AnimationObject> ("anim1");
    anim1.AddAnimationProperty<Quaternion> ("rotation", Mathf::EulerToQuaterion (Mathf::Deg2Rad (Vector3f (0.0f, 0.0f, 0.0f))), [&](Quaternion q) { plane.GetModelTransform ().Rotate (q); }, [](Quaternion q0, Quaternion q1, float t) { return Mathf::Slerp (q0, q1, Mathf::Smoothstep(0.0f, 1.0f, t)); });
    anim1.AddKeyframe<Quaternion> ("rotation", 2.0f, Mathf::EulerToQuaterion (Mathf::Deg2Rad (Vector3f (-35.0f, 0.0f, 0.0f))));
    anim1.AddKeyframe<Quaternion> ("rotation", 5.0f, Mathf::EulerToQuaterion (Mathf::Deg2Rad (Vector3f (-35.0f, 20.0f, -45.0f))));
    anim1.AddKeyframe<Quaternion> ("rotation", 11.0f, Mathf::EulerToQuaterion (Mathf::Deg2Rad (Vector3f (-35.0f, -20.0f, 45.0f))));
    anim1.AddKeyframe<Quaternion> ("rotation", 14.0f, Mathf::EulerToQuaterion (Mathf::Deg2Rad (Vector3f (-0.0f, 0.0f, 0.0f))));
    anim1.SetLooping (true);
    anim1.Play ();

    game.Run ();

    EngineContext::Deinitialize ();

    return 0;
}

