#include "GameLoop.h"
#include "GameScene.h"
#include "GameObject.h"
#include "MeshObject.h"

int main (int argc, char* argv[])
{
	GameScene scene;
	GLRenderer renderer;

	MeshObject& object = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	object.InitUnitCube ().getTransform ().Scale (0.3f, 0.3f, 0.3f).Translate (0.0f, 0.5f, 0.0f);

	MeshObject& object2 = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	object2.InitUnitPlane ().getTransform ().Scale (5.0f, 5.0f, 5.0f);

	GameLoop game (scene, renderer, 30.0f);
	game.Go ();

	return 0;
}