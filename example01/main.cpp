#include "cilantroengine.h"
#include "RotatingObject.h"

void main (int argc, char* argv[])
{
	GameScene scene;
	GLRenderer renderer (1280, 720);
	GameLoop game (scene, renderer);

	Material& green = scene.AddMaterial (new Material ());
	green.SetColor (Vector3f (0.1f, 0.4f, 0.1f));
	green.SetAmbientColor (Vector3f (0.5f, 0.5f, 0.5f));
	green.SetSpecularColor (Vector3f (1.0f, 1.0f, 1.0f)).SetSpecularShininess (64.0f);

	Material& red = scene.AddMaterial (new Material ());
	red.SetColor (Vector3f (0.75f, 0.1f, 0.1f));
	red.SetSpecularColor (Vector3f (1.0f, 0.0f, 0.0f)).SetSpecularShininess (8.0f);

	Material& lampM = scene.AddMaterial (new Material ());
	lampM.SetEmissiveColor (Vector3f (0.9f, 0.9f, 0.9f)).SetDiffuseColor (Vector3f (0.2f, 0.2f, 0.2f));

	Camera& cam = dynamic_cast<Camera&>(scene.AddGameObject (new Camera (Vector3f (1.0f, 2.0f, 5.0f), Vector3f (0.0f, -1.0f, 0.0f), Vector3f (0.0f, 1.0f, 0.0f), 75.0f, 0.1f, 100.0f)));
	scene.SetActiveCamera (&cam);

	RotatingObject& cube = dynamic_cast<RotatingObject&>(scene.AddGameObject (new RotatingObject ()));
	cube.InitUnitSphere (0).GetModelTransform ().Translate (0.0f, 0.0f, 0.0f).Rotate (0.0f, 0.0f, 25.0f);
	cube.SetMaterial (red);

	MeshObject& lamp = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	lamp.InitUnitCube ().GetModelTransform ().Scale (0.2f, 0.2f, 0.2f).Translate (1.0f, 0.75f, 1.0f);
	lamp.SetMaterial (lampM);
	lamp.SetParentObject (cube);

	MeshObject& plane = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	plane.InitUnitCube ().GetModelTransform ().Scale (5.0f, 0.1f, 5.0f).Translate (0.0f, -1.0f, 0.0f);
	plane.SetParentObject (cube);
	plane.SetMaterial (green);

	PointLight& light1 = dynamic_cast<PointLight&>(scene.AddGameObject (new PointLight ()));
	light1.SetParentObject (lamp);
	light1.SetLightColor (Vector3f (1.0f, 1.0f, 1.0f));
	light1.SetSpecularPower (0.7f);
	light1.SetAmbiencePower (0.1f);
	light1.SetLinearAttenuationFactor (0.2f).SetQuadraticAttenuationFactor (0.2f);
	light1.Enable ();

	game.Go ();
}

