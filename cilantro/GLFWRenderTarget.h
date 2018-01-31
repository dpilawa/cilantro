#ifndef _GLFWRENDERTARGET_H_
#define _GLFWRENDERTARGET_H_

#include "cilantroengine.h"
#include "RenderTarget.h"
#include "LogMessage.h"
#include "Time.h"

class GLFWRenderTarget : public RenderTarget
{
public:
	__EAPI GLFWRenderTarget (int xRes, int yRes, int fullscreen, int vsync);
	__EAPI ~GLFWRenderTarget ();

	__EAPI void Initialize ();
	__EAPI void Deinitialize ();

	__EAPI void BeforeFrame ();
	__EAPI void AfterFrame ();

private:

	// GL window context
	GLFWwindow * window;

	// parameters
	int isFullscreen;
	int vSync;

	// frame counters and timers
	float timeSinceLastSplit;
	float frameRenderTimeSinceLastSplit;
	float frameRenderTimeInLastSplit;
	long frameCount;
	long splitFrameCount;
	long lastFrameCount;

};

#endif
