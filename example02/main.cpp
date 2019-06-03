#include "cilantroengine.h"
#include "Orbiter.h"
#include "GameScene.h"
#include "GameLoop.h"
#include "PerspectiveCamera.h"
#include "MeshObject.h"
#include "PointLight.h"
#include "GLRenderer.h"
#include "GLFWRenderTarget.h"
#include "GLFWInputController.h"

int main (int argc, char* argv [])
{
	GameScene scene;

	GLFWRenderTarget target (1680, 720);
	target.SetDebugVisible (true);
	target.SetVSync (true);
	target.SetFullscreen (false);

	GLFWInputController controller (target.GetWindow ());

	GLRenderer renderer (scene, target);
	GameLoop game (scene, controller, renderer);

	controller.CreateInputEvent ("exit", InputKey::KeyEsc, InputTrigger::Press, {});
	controller.BindInputEvent ("exit", [ & ](float) { game.Stop (); });

	Material& sunM = scene.AddMaterial (new Material ());
	sunM.SetColor (Vector3f (0.7f, 0.7f, 0.7f));
	sunM.SetEmissiveColor (Vector3f (0.99f, 0.72f, 0.07f));

	Material& earthM = scene.AddMaterial (new Material ());
	earthM.SetColor (Vector3f (0.42f, 0.58f, 0.84f));
	earthM.SetSpecularColor (Vector3f (0.9f, 0.9f, 0.9f));

	Material& moonM = scene.AddMaterial (new Material ());
	moonM.SetDiffuseColor (Vector3f (0.3f, 0.3f, 0.3f));
	moonM.SetSpecularColor (Vector3f (0.0f, 0.0f, 0.0f));

	PerspectiveCamera& cam = dynamic_cast<PerspectiveCamera&>(scene.AddGameObject (new PerspectiveCamera (45.0f, 0.1f, 1000.0f)));
	cam.GetModelTransform ().Translate (0.0f, 0.0f, 70.0f);
	scene.SetActiveCamera (&cam);

	MeshObject& sun = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	sun.InitUnitSphere (5, true).GetModelTransform ().Scale (10.0f);
	sun.SetMaterial (sunM);

	Orbiter& earthOrbiter = dynamic_cast<Orbiter&>(scene.AddGameObject (new Orbiter (365.256f)));
	GameObject& earthOrbit = dynamic_cast<GameObject&>(scene.AddGameObject (new GameObject ()));
	earthOrbit.GetModelTransform ().Translate (0.0f, 0.0f, 50.0f);
	earthOrbit.SetParentObject (earthOrbiter);

	Orbiter& earthRotor = dynamic_cast<Orbiter&>(scene.AddGameObject (new Orbiter (1.0f)));
	earthRotor.GetModelTransform ().Rotate (0.0f, 0.0f, -23.0f);
	earthRotor.SetParentObject (earthOrbit);
	MeshObject& earth = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	earth.InitUnitSphere (5, true);
	earth.SetMaterial (earthM);
	earth.SetParentObject (earthRotor);

	Orbiter& moonOrbiter = dynamic_cast<Orbiter&>(scene.AddGameObject (new Orbiter (27.321f)));
	moonOrbiter.SetParentObject (earthOrbit);
	moonOrbiter.GetModelTransform ().Rotate (0.0f, 0.0f, 5.14f);
	MeshObject& moon = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	moon.InitUnitSphere (5, true).GetModelTransform ().Scale (0.273f).Translate (5.0f, 0.0f, 0.0f);
	moon.SetMaterial (moonM);
	moon.SetParentObject (moonOrbiter);

	PointLight& sunLight = dynamic_cast<PointLight&>(scene.AddGameObject (new PointLight ()));
	sunLight.SetParentObject (sun);
	sunLight.SetLightColor (Vector3f (1.0f, 1.0f, 1.0f));
	sunLight.SetSpecularPower (0.7f);
	sunLight.SetAmbiencePower (0.1f);
	sunLight.Enable ();

	game.Run ();

	return 0;
}

