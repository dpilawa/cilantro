#ifndef _GAMESCENE_H_
#define _GAMESCENE_H_

#include <functional>
#include <vector>
#include "GameObject.h"
#include "Material.h"
#include "LogMessage.h"
#include "Camera.h"

class GameLoop;

// This class represents a game world (a.k.a scene or level)
// It contains all visible and invisible objects in a game
class GameScene : public CallbackProvider<std::string, unsigned int>
{
public:
	GameScene();
	~GameScene();

	// add GameObject to a scene
	// returns reference to that added object
	GameObject& AddGameObject (GameObject* gameObject);

	// add material to the scene 
	// returns reference to that material
	Material& AddMaterial (Material* material);

	// return reference to GameObject vector
	std::vector <GameObject*>& GetGameObjects ();

	// return number of scene's GameObjects
	unsigned int getGameObjectsCount () const;

	// active camera manipulation
	void SetActiveCamera (Camera* camera);
	Camera* GetActiveCamera () const;

private:
	
	// vector of all GameObjects in the scene
	std::vector <GameObject*> gameObjects;

	// vector of all Materials in the scene
	std::vector <Material*> materials;

	// count of all GameObjects in the scene
	unsigned int gameObjectsCount;

	// reference to active camera
	Camera* activeCamera;

	// callback function to get notified about changed MeshObjects
	void OnModifiedMeshObject (unsigned int objectHandle);

};

#endif

