#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "GameScene.h"
#include "GameObject.h"

class Renderer
{
public:
	Renderer (GameScene& scene);
	~Renderer ();

	// abstract functions declarations
	virtual void Initialize () = 0;
	virtual void RenderFrame () = 0;
	virtual void Deinitialize () = 0;

	// callback function to get notified about new or modified GameObjects
	virtual void OnUpdateGameObject (size_t objectHandle) = 0;

protected:
	GameScene& renderedScene;
};

#endif
