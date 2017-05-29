#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "GameScene.h"
#include "GameObject.h"

class Renderer
{
public:
	Renderer (GameScene& scene);
	~Renderer ();

	virtual void OnStart () = 0;
	virtual void OnFrame () = 0;
	virtual void OnEnd () = 0;

	virtual void OnUpdateObject (GameObject*) = 0;

protected:
	GameScene& renderedScene;
};

#endif
