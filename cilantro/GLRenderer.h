#ifndef _GLRENDERER_H_
#define _GLRENDERER_H_

#include "Renderer.h"
#include "GameScene.h"
#include "GameObject.h"
#include "MeshObject.h"
#include "GLFW/glfw3.h"

class GLRenderer : public Renderer
{
public:
	GLRenderer (int xRes, int yRes);
	~GLRenderer ();

	void OnStart ();
	void OnFrame ();
	void OnEnd ();

private:
	// rendering resolution
	int xResolution;
	int yResolution;

	// GL window context
	GLFWwindow* window;
};

#endif