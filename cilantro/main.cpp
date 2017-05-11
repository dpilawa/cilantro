#include "GameScene.h"
#include "GameObject.h"
#include "MeshObject.h"

int main (int argc, char* argv[])
{
	GameScene scene;

	GameObject& object = scene.AddGameObject (dynamic_cast<GameObject*>(new MeshObject ()));
	object.getTransform ().Scale (0.3f, 0.3f, 0.3f);

	GameObject& object2 = scene.AddGameObject (dynamic_cast<GameObject*>(new MeshObject ()));
	object2.getTransform ().Scale (0.1f, 0.1f, 0.1f).Translate (1.0f, 0.0f, 0.0f);


	scene.Run ();

	return 0;
}