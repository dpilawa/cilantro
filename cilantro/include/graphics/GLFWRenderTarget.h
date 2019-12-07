#ifndef _GLFWRENDERTARGET_H_
#define _GLFWRENDERTARGET_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "game/GameLoop.h"
#include "graphics/RenderTarget.h"
#include "graphics/GLRenderer.h"
#include <string>

class GLFWRenderTarget : public RenderTarget
{
public:
	__EAPI GLFWRenderTarget () = delete;
	__EAPI GLFWRenderTarget (GameLoop* gameLoop, std::string windowCaption,  unsigned int width, unsigned int height, bool isFullscreen, bool isResizable, bool isVSync);
	__EAPI ~GLFWRenderTarget ();

	__EAPI void OnFrame ();

	__EAPI GLFWwindow** GetWindow ();

private:

	void Initialize ();
	void Deinitialize ();
	void FramebufferResizeCallback (int width, int height);

	// GL window context
	GLFWwindow* window;

	// GL renderer
	GLRenderer* glRenderer;

	// VBO and VAO
	GLuint targetVAO;
	GLuint targetVBO;

	// parameters
	std::string windowCaption;
	bool isFullscreen;
	bool isResizable;
	bool isVSync;

};

#endif
