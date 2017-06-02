#ifndef _GLRENDERER_H_
#define _GLRENDERER_H_

#include <vector>
#include "Renderer.h"
#include "GLFW/glfw3.h"

class GLRenderer : public Renderer
{
public:
	GLRenderer (GameScene& scene,  int xRes, int yRes);
	~GLRenderer ();

	// initialize renderer
	void Initialize ();
	// render one frame
	void RenderFrame ();
	// deinitialize renderer
	void Deinitialize ();

private:
	// rendering resolution
	int xResolution;
	int yResolution;

	// GL window context
	GLFWwindow* window;

	// VAOs and EBOs for all scene objects
	std::vector <GLint> VAO;
	std::vector <GLint> EBO;

	// callback function to get notified about new or modified GameObjects
	void OnUpdateGameObject (size_t objectHandle);
};

#endif