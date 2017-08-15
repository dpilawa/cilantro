#include "GameLoop.h"
#include "GameScene.h"
#include "GameObject.h"
#include "MeshObject.h"
#include "PointLight.h"
#include "Camera.h"
#include "GLRenderer.h"
#include "LogMessage.h"

void main (int argc, char* argv[])
{
	GameScene scene;
	GLRenderer renderer (1280, 720);
	GameLoop game (scene, renderer);

	Material& green = scene.AddMaterial (new Material ());
	green.SetDiffuseColor (Vector3f (0.1f, 0.75f, 0.1f));

	Material& red = scene.AddMaterial (new Material ());
	red.SetDiffuseColor (Vector3f (0.75f, 0.1f, 0.1f));

	Camera& cam = dynamic_cast<Camera&>(scene.AddGameObject (new Camera (Vector3f (1.0f, 2.0f, 5.0f), Vector3f (0.0f, -1.0f, 0.0f), Vector3f (0.0f, 1.0f, 0.0f), 75.0f, 0.1f, 100.0f)));
	scene.SetActiveCamera (&cam);

	MeshObject& cube = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	cube.InitUnitCube ().GetModelTransform ().Translate (0.0f, 0.0f, 0.0f).Rotate (0.0f, 0.0f, 25.0f);
	cube.SetMaterial (red);

	MeshObject& plane = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	plane.InitUnitCube ().GetModelTransform ().Scale (5.0f, 0.1f, 5.0f).Translate (0.0f, -1.0f, 0.0f);
	plane.SetParentObject (cube);
	plane.SetMaterial (green);

	PointLight& light1 = dynamic_cast<PointLight&>(scene.AddGameObject (new PointLight ()));
	light1.GetModelTransform ().Translate (2.0f, 4.0f, 3.0f);
	light1.SetLightColor (Vector3f (1.0f, 1.0f, 0.0f));
	light1.Enable ();

	PointLight& light2 = dynamic_cast<PointLight&>(scene.AddGameObject (new PointLight ()));
	light2.SetLightColor (Vector3f (0.1f, 0.0f, 1.0f));
	light2.GetModelTransform ().Translate (10.0f, 10.0f, 10.0f);
	light2.Enable ();

	game.Go ();
}

