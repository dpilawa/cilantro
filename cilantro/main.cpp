#include "GameLoop.h"
#include "GameScene.h"
#include "GameObject.h"
#include "MeshObject.h"
#include "GLRenderer.h"
#include "LogMessage.h"

void main (int argc, char* argv[])
{
	LogMessage (__FUNCTION__) << "Engine starting";

	GameScene scene;
	GLRenderer renderer (scene, 800, 600);
	GameLoop game (scene, renderer);

	MeshObject& cube = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	cube.InitUnitCube ().GetModelTransform ().Translate (0.0f, 0.0f, 0.0f).Rotate (0.0f, 0.0f, 0.0f);

	MeshObject& plane = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	plane.InitUnitCube ().GetModelTransform ().Scale (5.0f, 0.1f, 5.0f).Translate (0.0f, -1.0f, 0.0f);
	//plane.SetParentObject (cube);

	game.Go ();

	LogMessage (__FUNCTION__) << "Engine shutdown";
}