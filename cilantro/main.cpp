#include "GameScene.h"
#include "GameObject.h"
#include "MeshObject.h"

int main (int argc, char* argv[])
{
	GameScene scene;
	MeshObject* cube = new MeshObject ();

	cube->InitUnitCube ();
	scene.AddGameObject (cube);
	scene.Run ();

	return 0;
}