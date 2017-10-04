#include "cilantroengine.h"
#include "Orbiter.h"

void main (int argc, char* argv [])
{
	GameScene scene;
	GLRenderer renderer (1680, 720);
	GameLoop game (scene, renderer);

	Material& sunM = scene.AddMaterial (new Material ());
	sunM.SetColor (Vector3f (0.7f, 0.7f, 0.7f));
	sunM.SetEmissiveColor (Vector3f (0.99f, 0.72f, 0.07f));

	Material& earthM = scene.AddMaterial (new Material ());
	earthM.SetColor (Vector3f (0.42f, 0.58f, 0.84f));
	earthM.SetSpecularColor (Vector3f (0.9f, 0.9f, 0.9f));

	Material& moonM = scene.AddMaterial (new Material ());
	moonM.SetDiffuseColor (Vector3f (0.3f, 0.3f, 0.3f));
	moonM.SetSpecularShininess (64.0f);

	Camera& cam = dynamic_cast<Camera&>(scene.AddGameObject (new Camera (Vector3f (10.0f, 0.0f, 70.0f), Vector3f (0.0f, 0.0f, 0.0f), Vector3f (0.0f, 1.0f, 0.0f), 45.0f, 0.1f, 1000.0f)));
	scene.SetActiveCamera (&cam);

	MeshObject& sun = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	sun.InitUnitSphere (6).GetModelTransform ().Scale (10.0f);
	sun.SetMaterial (sunM);

	Orbiter& earthOrbiter = dynamic_cast<Orbiter&>(scene.AddGameObject (new Orbiter (365.256f)));
	GameObject& earthOrbit = dynamic_cast<GameObject&>(scene.AddGameObject (new GameObject ()));
	earthOrbit.GetModelTransform ().Translate (0.0f, 0.0f, 50.0f);
	earthOrbit.SetParentObject (earthOrbiter);

	Orbiter& earthRotor = dynamic_cast<Orbiter&>(scene.AddGameObject (new Orbiter (1.0f)));
	earthRotor.GetModelTransform ().Rotate (0.0f, 0.0f, -23.0f);
	earthRotor.SetParentObject (earthOrbit);
	MeshObject& earth = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	earth.InitUnitSphere (4);
	earth.SetMaterial (earthM);
	earth.SetParentObject (earthRotor);

	Orbiter& moonOrbiter = dynamic_cast<Orbiter&>(scene.AddGameObject (new Orbiter (27.321f)));
	moonOrbiter.SetParentObject (earthOrbit);
	moonOrbiter.GetModelTransform ().Rotate (0.0f, 0.0f, 5.14f);
	MeshObject& moon = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	moon.InitUnitSphere (3, true).GetModelTransform ().Scale (0.273f).Translate (5.0f, 0.0f, 0.0f);
	moon.SetMaterial (moonM);
	moon.SetParentObject (moonOrbiter);

	PointLight& sunLight = dynamic_cast<PointLight&>(scene.AddGameObject (new PointLight ()));
	sunLight.SetParentObject (sun);
	sunLight.SetLightColor (Vector3f (1.0f, 1.0f, 1.0f));
	sunLight.SetSpecularPower (0.7f);
	sunLight.SetAmbiencePower (0.1f);
	sunLight.Enable ();

	game.Go ();
}

