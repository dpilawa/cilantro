#ifndef _GLRENDERER_H_
#define _GLRENDERER_H_

#include <unordered_map>
#include "Time.h"
#include "Renderer.h"
#include "LogMessage.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

struct Buffers
{
public:
	GLint VBO;
	GLint EBO;
	GLint VAO;
};

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

	// GL buffers and arrays for all scene objects
	std::unordered_map <unsigned int, Buffers> buffers;

	// initialize object buffers
	void InitializeBuffers ();

	// re(load) object buffers
	void LoadBuffers (unsigned int objectHandle);

	// callback function to get notified about new or modified GameObjects
	void OnUpdateGameObject (unsigned int objectHandle);
};

#endif