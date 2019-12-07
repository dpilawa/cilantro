#include "cilantroengine.h"
#include "game/GameLoop.h"
#include "scene/GameScene.h"
#include "scene/PerspectiveCamera.h"
#include "scene/MeshObject.h"
#include "scene/PointLight.h"
#include "graphics/GLRenderer.h"
#include "graphics/GLFWRenderTarget.h"
#include "input/GLFWInputController.h"

#include "Orbiter.h"

int main (int argc, char* argv [])
{
	GameLoop game;

	GameScene scene (&game);
	game.gameScene = &scene;

	GLFWRenderTarget target (&game, "Test 4", 960, 600, false, true, true);
	game.gameRenderTarget = &target;

	GLFWInputController controller (&game, target.GetWindow ());
	game.gameInputController = &controller;

    GLRenderer renderer (&game, 960, 600);
	game.gameRenderer = &renderer;

	controller.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
	controller.BindInputEvent ("exit", [ & ]() { game.Stop (); });

	Material& sunM = scene.AddMaterial (new Material ());
	sunM.SetColor (Vector3f (0.7f, 0.7f, 0.7f));
	sunM.SetEmissiveColor (Vector3f (0.99f, 0.72f, 0.07f));

	Material& earthM = scene.AddMaterial (new Material ());
	earthM.SetColor (Vector3f (0.42f, 0.58f, 0.84f));
	earthM.SetSpecularColor (Vector3f (0.9f, 0.9f, 0.9f));

	Material& moonM = scene.AddMaterial (new Material ());
	moonM.SetDiffuseColor (Vector3f (0.3f, 0.3f, 0.3f));
	moonM.SetSpecularColor (Vector3f (0.0f, 0.0f, 0.0f));

	PerspectiveCamera& cam = dynamic_cast<PerspectiveCamera&>(scene.AddGameObject (new PerspectiveCamera (25.0f, 1.0f, 500.0f)));
	cam.GetModelTransform ().Translate (0.0f, 0.0f, 160.0f);
	scene.SetActiveCamera (&cam);

	MeshObject& sun = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	sun.InitUnitSphere (8, true).GetModelTransform ().Scale (10.0f);
	sun.SetMaterial (sunM);

	Orbiter& earthOrbit = dynamic_cast<Orbiter&>(scene.AddGameObject (new Orbiter (365.256f, 50.0f, 0.0f)));
	Orbiter& earthRotor = dynamic_cast<Orbiter&>(scene.AddGameObject (new Orbiter (1.0f, 0.0f, -23.0f)));
    earthRotor.SetParentObject (earthOrbit);

    MeshObject& earth = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	earth.InitUnitSphere (5, true);
	earth.SetMaterial (earthM);
	earth.SetParentObject (earthRotor);

	Orbiter& moonOrbit = dynamic_cast<Orbiter&>(scene.AddGameObject (new Orbiter (27.321f, 5.0f, 15.14f)));
    moonOrbit.SetParentObject (earthOrbit);

    MeshObject& moon = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
    moon.InitUnitSphere (5, true).GetModelTransform ().Scale (0.273f);
    moon.SetMaterial (moonM);
    moon.SetParentObject (moonOrbit);

    PointLight& sunLight = dynamic_cast<PointLight&>(scene.AddGameObject (new PointLight ()));
	sunLight.SetParentObject (sun);
	sunLight.SetColor (Vector3f (1.0f, 1.0f, 1.0f));
	sunLight.SetSpecularPower (0.7f);
	sunLight.SetAmbiencePower (0.1f);
	sunLight.SetEnabled (true);

	game.Run ();

	return 0;
}

