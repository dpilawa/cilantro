#include "GameLoop.h"
#include "GameScene.h"
#include "GameObject.h"
#include "MeshObject.h"
#include "Camera.h"
#include "GLRenderer.h"
#include "LogMessage.h"

void main (int argc, char* argv[])
{
	GameScene scene;
	GLRenderer renderer (1280, 720);
	GameLoop game (scene, renderer);

	Camera& cam = dynamic_cast<Camera&>(scene.AddGameObject (new Camera (Vector3f (1.0f, 2.0f, 5.0f), Vector3f (0.0f, -1.0f, 0.0f), Vector3f (0.0f, 1.0f, 0.0f), 75.0f, 0.1f, 100.0f)));
	scene.SetActiveCamera (&cam);

	MeshObject& cube = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	cube.InitUnitCube ().GetModelTransform ().Translate (0.0f, 0.0f, 0.0f).Rotate (0.0f, 0.0f, 25.0f);

	MeshObject& plane = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	plane.InitUnitCube ().GetModelTransform ().Scale (5.0f, 0.1f, 5.0f).Translate (0.0f, -1.0f, 0.0f);
	plane.SetParentObject (cube);

	game.Go ();
}

