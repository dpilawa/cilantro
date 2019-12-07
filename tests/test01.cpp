#include "cilantroengine.h"
#include "game/GameLoop.h"
#include "scene/AnimationObject.h"
#include "scene/GameScene.h"
#include "scene/MeshObject.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "scene/SpotLight.h"
#include "graphics/GLRenderer.h"
#include "graphics/GLFWRenderTarget.h"
#include "input/GLFWInputController.h"
#include "math/Mathf.h"
#include "util/LogMessage.h"

#include "ControlledCamera.h"
#include "RotatingObject.h"

#include <iostream>

int main (int argc, char* argv[])
{
	GameLoop game;

	GameScene scene (&game);
	game.gameScene = &scene;

	GLFWRenderTarget target (&game, "Test 1", 800, 600, false, true, true);
	game.gameRenderTarget = &target;

	GLFWInputController controller (&game, target.GetWindow ());
	game.gameInputController = &controller;

    GLRenderer renderer (&game, 800, 600);
	game.gameRenderer = &renderer;

	controller.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
	controller.BindInputEvent ("exit", [ & ]() { game.Stop (); });

	controller.CreateInputEvent ("mousemode", InputKey::KeySpace, InputTrigger::Release, {});
	controller.BindInputEvent ("mousemode", [ & ]() { controller.SetMouseGameMode (!controller.IsGameMode ()); });

    Material& green = scene.AddMaterial (new Material ());
	green.SetShaderProgram ("blinnphong_shader");
	green.SetColor (Vector3f (0.1f, 0.4f, 0.1f));
	green.SetAmbientColor (Vector3f (0.5f, 0.5f, 0.5f));
	green.SetSpecularColor (Vector3f (1.0f, 1.0f, 1.0f)).SetSpecularShininess (32.0f);

	Material& red = scene.AddMaterial (new Material ());
    red.SetShaderProgram ("phong_shader");
    red.SetColor (Vector3f (0.75f, 0.1f, 0.1f));
    red.SetSpecularColor (Vector3f (1.0f, 0.0f, 0.0f)).SetSpecularShininess (8.0f);

	Material& lampM = scene.AddMaterial (new Material ());
	lampM.SetEmissiveColor (Vector3f (0.9f, 0.9f, 0.9f)).SetDiffuseColor (Vector3f (0.2f, 0.2f, 0.2f));

	ControlledCamera& cam = dynamic_cast<ControlledCamera&>(scene.AddGameObject (new ControlledCamera (60.0f, 0.1f, 100.0f, 0.1f)));
    cam.Initialize ();
    cam.GetModelTransform ().Translate (5.0f, 1.5f, 5.0f).Rotate (-20.0f, 45.0f, 0.0f);
    scene.SetActiveCamera (&cam);

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

