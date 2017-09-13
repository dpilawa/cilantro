#include "cilantroengine.h"
#include "Orbiter.h"

void main (int argc, char* argv [])
{
	GameScene scene;
	GLRenderer renderer (1280, 720);
	GameLoop game (scene, renderer);

	Material& gray = scene.AddMaterial (new Material ());
	gray.SetColor (Vector3f (0.7f, 0.7f, 0.7f));

	Camera& cam = dynamic_cast<Camera&>(scene.AddGameObject (new Camera (Vector3f (3.0f, 5.0f, 15.0f), Vector3f (0.0f, 0.0f, 0.0f), Vector3f (0.0f, 1.0f, 0.0f), 60.0f, 0.1f, 100.0f)));
	scene.SetActiveCamera (&cam);

	MeshObject& sun = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	sun.InitUnitCube ();
	sun.SetMaterial (gray);

	Orbiter& earthOrbiter = dynamic_cast<Orbiter&>(scene.AddGameObject (new Orbiter (365.0f)));
	MeshObject& earth = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	earth.InitUnitCube ().GetModelTransform ().Scale (0.3f, 0.3f, 0.3f).Translate(5.0f, 0.0f, 0.0f);
	earth.SetMaterial (gray);
	earth.SetParentObject (earthOrbiter);

	Orbiter& moonOrbiter = dynamic_cast<Orbiter&>(scene.AddGameObject (new Orbiter (28.0f)));
	moonOrbiter.SetParentObject (earth);
	MeshObject& moon = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	moon.InitUnitCube ().GetModelTransform ().Scale (0.2f, 0.2f, 0.2f).Translate (2.0f, 0.0f, 0.0f);
	moon.SetMaterial (gray);
	moon.SetParentObject (moonOrbiter);

	PointLight& sunLight = dynamic_cast<PointLight&>(scene.AddGameObject (new PointLight ()));
	sunLight.SetParentObject (sun);
	sunLight.SetLightColor (Vector3f (1.0f, 1.0f, 1.0f));
	sunLight.SetSpecularPower (0.7f);
	sunLight.SetAmbiencePower (0.1f);
	sunLight.Enable ();

	game.Go ();
}

