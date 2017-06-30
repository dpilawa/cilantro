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
	cube.InitUnitCube ().GetWorldTransform ().Scale (0.3f, 0.3f, 0.3f).Translate (0.0f, 0.5f, 0.0f);

	//MeshObject& plane = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	//plane.InitUnitPlane ().GetWorldTransform ().Scale (5.0f, 5.0f, 5.0f);
	//plane.SetParentObject (cube);

	game.Go ();

	LogMessage (__FUNCTION__) << "Engine shutdown";
}