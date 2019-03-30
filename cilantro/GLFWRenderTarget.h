#ifndef _GLFWRENDERTARGET_H_
#define _GLFWRENDERTARGET_H_

#include "cilantroengine.h"
#include "GLFW/glfw3.h"
#include "RenderTarget.h"

class GLFWRenderTarget : public RenderTarget
{
public:
	__EAPI GLFWRenderTarget (int xRes, int yRes);
	__EAPI ~GLFWRenderTarget ();

	__EAPI void Initialize ();
	__EAPI void Deinitialize ();

	__EAPI void BeforeFrame ();
	__EAPI void AfterFrame ();

	__EAPI void setFullscreen (bool fullscreen);
	__EAPI void setResizable (bool resizable);
	__EAPI void setVSync (bool vsync);
	__EAPI void setDebugVisible (bool debugvisible);

private:

	// GL window context
	GLFWwindow * window;

	// parameters
	bool isFullscreen;
	bool isResizable;
	bool isVSync;
	bool isDebugVisible;

	// frame counters and timers
	float timeSinceLastSplit;
	float frameRenderTimeSinceLastSplit;
	float frameRenderTimeInLastSplit;
	long frameCount;
	long splitFrameCount;
	long lastFrameCount;

};

#endif
