#ifndef _RENDERTARGET_H_
#define _RENDERTARGET_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class RenderTarget
{
public:
	RenderTarget (int xRes, int yRes);
	~RenderTarget ();

	virtual void Initialize () = 0;
	virtual void Deinitialize () = 0;
	virtual void BeforeFrame () = 0;
	virtual void AfterFrame () = 0;

	int GetXResolution ();
	int GetYResolution ();

protected:

	// rendering resolution
	int xResolution;
	int yResolution;

};

#endif