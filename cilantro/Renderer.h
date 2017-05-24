#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "GameObject.h"
#include "GameScene.h"

class Renderer
{
public:
	Renderer ();
	~Renderer ();

	virtual void OnStart () = 0;
	virtual void OnFrame () = 0;
	virtual void OnEnd () = 0;

	virtual void DrawGameObject (GameObject& gameObject) = 0;
private:
protected:
	GameScene & gameScene;
};

#endif
