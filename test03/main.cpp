#include "cilantroengine.h"
#include "AnimationObject.h"
#include "GameScene.h"
#include "GameLoop.h"
#include "MeshObject.h"
#include "PerspectiveCamera.h"
#include "DirectionalLight.h"
#include "GLRenderer.h"
#include "GLFWRenderTarget.h"
#include "GLFWInputController.h"
#include "Mathf.h"
#include "LogMessage.h"

#include <iostream>

int main (int argc, char* argv[])
{
	GameScene scene;
	
	GLFWRenderTarget target;
    target.SetResolution (800, 600);
    target.SetDebugVisible (true);
    target.SetVSync (true);
	target.SetResizable (true);
	target.SetFullscreen (false);

	GLFWInputController controller;
    controller.SetGLFWWindow (target.GetWindow ());

    GLRenderer renderer;
	GameLoop game (scene, controller, renderer, target);

	controller.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
	controller.BindInputEvent ("exit", [ & ]() { game.Stop (); });

	controller.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
	controller.BindInputEvent ("mousemode", [ & ]() { controller.SetMouseGameMode (!controller.IsGameMode ()); });

    Material& m = scene.AddMaterial (new Material ());
	m.SetShaderModelName ("blinnphong_shader");
	m.SetColor (Vector3f (0.4f, 0.4f, 0.4f));
	m.SetAmbientColor (Vector3f (1.0f, 1.0f, 1.0f));
	m.SetSpecularColor (Vector3f (1.0f, 1.0f, 1.0f)).SetSpecularShininess (64.0f);

    Material& red = scene.AddMaterial (new Material ());
	red.SetShaderModelName ("emissive_shader");
	red.SetEmissiveColor (Vector3f (1.0f, 0.0f, 0.0f));

	Material& green = scene.AddMaterial (new Material ());
	green.SetShaderModelName ("emissive_shader");
	green.SetEmissiveColor (Vector3f (0.0f, 1.0f, 0.0f));

	Material& blue = scene.AddMaterial (new Material ());
	blue.SetShaderModelName ("emissive_shader");
	blue.SetEmissiveColor (Vector3f (0.0f, 0.0f, 1.0f));

	PerspectiveCamera& cam = dynamic_cast<PerspectiveCamera&>(scene.AddGameObject (new PerspectiveCamera (45.0f, 0.1f, 100.0f)));
    cam.GetModelTransform ().Translate (15.0f, 15.0f, 15.0f).Rotate (-35.0f, 45.0f, 0.0f);
    scene.SetActiveCamera (&cam);

	MeshObject& x = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject (MeshType::Lines)));
    x.AddVertex (Vector3f (0.0f, 0.0f, 0.0f));
    x.AddVertex (Vector3f (10.0f, 0.0f, 0.0f));
    x.AddLine (0, 1);
    x.SetMaterial (red);

    MeshObject& y = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject (MeshType::Lines)));
    y.AddVertex (Vector3f (0.0f, 0.0f, 0.0f));
    y.AddVertex (Vector3f (0.0f, 10.0f, 0.0f));
    y.AddLine (0, 1);
    y.SetMaterial (green);

	MeshObject& z = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject (MeshType::Lines)));
    z.AddVertex (Vector3f (0.0f, 0.0f, 0.0f));
    z.AddVertex (Vector3f (0.0f, 0.0f, 10.0f));
    z.AddLine (0, 1);
    z.SetMaterial (blue);

    GameObject& plane = scene.AddGameObject (new GameObject ());

    MeshObject& fuselage = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    fuselage.GetModelTransform ().Scale (1.0f, 1.0f, 10.0f);
    fuselage.InitUnitCube ();
    fuselage.SetMaterial (m);
    fuselage.SetParentObject (plane);

    MeshObject& wings = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    wings.GetModelTransform ().Scale (12.0f, 0.2f, 1.0f).Translate (0.0f, 0.0f, 2.0f);
    wings.InitUnitCube ();
    wings.SetMaterial (m);
    wings.SetParentObject (plane);

    MeshObject& rudder = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    rudder.GetModelTransform ().Scale (0.2f, 2.0f, 1.0f).Translate (0.0f, 1.0f, -5.0f).Rotate (-10.0f, 0.0f, 0.0f);
    rudder.InitUnitCube ();
    rudder.SetMaterial (m);
    rudder.SetParentObject (plane);

    MeshObject& tail = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    tail.GetModelTransform ().Scale (4.0f, 0.2f, 1.0f).Translate (0.0f, 0.2f, -5.0f);
    tail.InitUnitCube ();
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

