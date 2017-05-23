#ifndef _GLRENDERER_H_
#define _GLRENDERER_H_

#include "Renderer.h"
#include "GLFW/glfw3.h"

class GLRenderer : public Renderer
{
public:
	GLRenderer ();
	~GLRenderer ();

	void OnStart ();
	void OnFrame ();
	void OnEnd ();
private:
	// GL window context
	GLFWwindow* window;


};

#endif