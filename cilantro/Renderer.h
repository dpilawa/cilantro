#ifndef _RENDERER_H_
#define _RENDERER_H_

class MeshObject;
class GameScene;

class Renderer
{
public:
	Renderer (GameScene& scene);
	~Renderer ();

	// abstract functions declarations
	virtual void Initialize () = 0;
	virtual void RenderFrame ();
	virtual void Deinitialize () = 0;

	// object drawing functions
	virtual void Draw (MeshObject& meshobject) = 0;

	// return number of rendered frames
	long long GetFrameCount () const;

protected:
	// GameScene being rendered
	GameScene& renderedScene;

	// number of rendered frames
	long long frameCount;
};

#endif
