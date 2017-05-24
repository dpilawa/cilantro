#include "GameLoop.h"
#include "GameScene.h"
#include "GameObject.h"
#include "MeshObject.h"
#include "GLRenderer.h"

int main (int argc, char* argv[])
{
	GameScene scene;
	GLRenderer renderer (800, 600);

	MeshObject& object = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	object.InitUnitCube ().getWorldTransform ().Scale (0.3f, 0.3f, 0.3f).Translate (0.0f, 0.5f, 0.0f);

	MeshObject& object2 = dynamic_cast<MeshObject&>(scene.AddGameObject (new MeshObject ()));
	object2.InitUnitPlane ().getWorldTransform ().Scale (5.0f, 5.0f, 5.0f);

	GameLoop game (scene, renderer);
	game.Go ();

	return 0;
}