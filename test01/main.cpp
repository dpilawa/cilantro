#include "cilantroengine.h"
#include "RotatingObject.h"
#include "GameScene.h"
#include "GameLoop.h"
#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"
#include "MeshObject.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "GLRenderer.h"
#include "GLFWRenderTarget.h"
#include "GLFWInputController.h"
#include "Mathf.h"
#include "LogMessage.h"

#include <iostream>

int main (int argc, char* argv[])
{
	GameScene scene;
	
	GLFWRenderTarget target (800, 600);
	target.SetDebugVisible (true);
	target.SetVSync (true);
	target.SetResizable (true);
	target.SetFullscreen (false);

	GLFWInputController controller (target.GetWindow ());

	GLRenderer renderer (scene, target);
	GameLoop game (scene, controller, renderer);

	controller.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
	controller.BindInputEvent ("exit", [ & ]() { game.Stop (); });

	controller.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
	controller.BindInputEvent ("mousemode", [ & ]() { controller.SetMouseGameMode (!controller.IsGameMode ()); });

    controller.CreateInputAxis ("moveforward", InputKey::KeyW, {}, 1.0f);
    controller.CreateInputAxis ("moveforward", InputKey::KeyS, {}, -1.0f);	

	controller.CreateInputAxis ("moveright", InputKey::KeyD, {}, 1.0f);
    controller.CreateInputAxis ("moveright", InputKey::KeyA, {}, -1.0f);

    controller.CreateInputAxis ("camerapitch", InputAxis::MouseY, 1.0f);
    controller.CreateInputAxis ("camerayaw", InputAxis::MouseX, 1.0f);
	

    Material& green = scene.AddMaterial (new Material ());
	green.SetShaderModelName ("blinnphong_shader");
	green.SetColor (Vector3f (0.1f, 0.4f, 0.1f));
	green.SetAmbientColor (Vector3f (0.5f, 0.5f, 0.5f));
	green.SetSpecularColor (Vector3f (1.0f, 1.0f, 1.0f)).SetSpecularShininess (32.0f);

	Material& red = scene.AddMaterial (new Material ());
    red.SetShaderModelName ("phong_shader");
    red.SetColor (Vector3f (0.75f, 0.1f, 0.1f));
    red.SetSpecularColor (Vector3f (1.0f, 0.0f, 0.0f)).SetSpecularShininess (8.0f);

	Material& lampM = scene.AddMaterial (new Material ());
	lampM.SetEmissiveColor (Vector3f (0.9f, 0.9f, 0.9f)).SetDiffuseColor (Vector3f (0.2f, 0.2f, 0.2f));

	PerspectiveCamera& cam = dynamic_cast<PerspectiveCamera&>(scene.AddGameObject (new PerspectiveCamera (60.0f, 0.1f, 100.0f)));
	//OrthographicCamera& cam = dynamic_cast<OrthographicCamera&>(scene.AddGameObject (new OrthographicCamera (10.0f, 0.1f, 100.0f)));
	cam.GetModelTransform ().Translate (5.0f, 1.5f, 5.0f).Rotate (-20.0f, 45.0f, 0.0f);
	scene.SetActiveCamera (&cam);

    controller.BindInputAxis ("moveright", [&](float a) { cam.GetModelTransform ().TranslateBy (cam.GetRight () * a * 0.1f); });
	controller.BindInputAxis ("moveforward", [&](float a) { cam.GetModelTransform ().TranslateBy (-cam.GetForward () * a * 0.1f); });
	controller.BindInputAxis ("camerapitch", [&](float a) { cam.GetModelTransform ().RotateBy (a * 0.25f, 0.0f, 0.0f); });
	controller.BindInputAxis ("camerayaw", [&](float a) { cam.GetModelTransform ().RotateBy (0.0f, a * 0.25f, 0.0f); });

    MeshObject& cube = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	cube.InitUnitCube ();
	cube.SetMaterial (red);

	RotatingObject& lampPivot = dynamic_cast<RotatingObject&>(scene.AddGameObject (new RotatingObject ()));
	MeshObject& lamp = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	lamp.InitUnitSphere (3).GetModelTransform ().Scale (0.1f, 0.1f, 0.1f).Translate (1.0f, 0.75f, 1.0f);
	lamp.SetMaterial (lampM);
	lamp.SetParentObject (lampPivot);

	MeshObject& plane = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	plane.InitUnitCube ().GetModelTransform ().Scale (5.0f, 0.1f, 5.0f).Translate (0.0f, -1.0f, 0.0f);
	plane.SetParentObject (cube);
	plane.SetMaterial (green);

	PointLight& light1 = dynamic_cast<PointLight&>(scene.AddGameObject (new PointLight ()));
	light1.SetParentObject (lamp);
	light1.SetColor (Vector3f (1.0f, 1.0f, 1.0f));
	light1.SetSpecularPower (0.7f);
	light1.SetAmbiencePower (0.1f);
	light1.SetLinearAttenuationFactor (0.2f).SetQuadraticAttenuationFactor (0.2f);
	light1.SetEnabled (true);

	DirectionalLight& light2 = dynamic_cast<DirectionalLight&>(scene.AddGameObject (new DirectionalLight ()));
	light2.GetModelTransform ().Rotate (135.0f, -45.0f, 0.0f);
	light2.SetColor (Vector3f (0.7f, 0.7f, 0.7f));
	light2.SetSpecularPower (2.0f);
	light2.SetAmbiencePower (0.0f);
	light2.SetEnabled (true);

	SpotLight& light3 = dynamic_cast<SpotLight&>(scene.AddGameObject (new SpotLight ()));
	light3.GetModelTransform ().Translate (2.0f, 10.0f, 0.0f).Rotate (90.0f, 0.0f, 0.0f);
	light3.SetColor (Vector3f (1.0f, 1.0f, 1.0f));
	light3.SetSpecularPower (1.0f);
    light3.SetInnerCutoff (5.0f);
	light3.SetOuterCutoff (12.0f);
    light3.SetEnabled (true);

    game.Run ();

	return 0;
}

