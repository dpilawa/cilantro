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

	void OnStart ();
	void OnFrame ();
	void OnEnd ();

	void OnUpdateObject (GameObject*);

private:
	// rendering resolution
	int xResolution;
	int yResolution;

	// GL window context
	GLFWwindow* window;

	// VAOs and EBOs for all scene objects
	std::vector <GLint> vao;
	std::vector <GLint> ebo;
};

#endif