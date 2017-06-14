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
	virtual void RenderFrame ();
	virtual void Deinitialize () = 0;

	// callback function to get notified about new or modified GameObjects
	virtual void OnUpdateGameObject (unsigned int objectHandle) = 0;

	// return number of rendered frames
	long long GetFrameCount () const;

protected:
	// GameScene being rendered
	GameScene& renderedScene;

	// number of rendered frames
	long long frameCount;
};

#endif
