#include "GLRenderer.h"

GLRenderer::GLRenderer (GameScene & scene, int xRes, int yRes) : xResolution (xRes), yResolution (yRes)
{
	gameScene = scene;

	glfwInit ();
}


GLRenderer::~GLRenderer ()
{
	glfwTerminate ();
}

void GLRenderer::OnStart ()
{
	// set up GL & window properties
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint (GLFW_RESIZABLE, GL_FALSE);
	
	// create window
	window = glfwCreateWindow (xResolution, yResolution, "GL", nullptr, nullptr);

	// make openGL context active
	glfwMakeContextCurrent (window);

	// set vsync on
	glfwSwapInterval (1);
}

void GLRenderer::OnFrame ()
{
	// traverse GameObject tree and draw objects
	for each (GameObject* g in gameScene.GetGameObjectsVector())
	{
		g->OnDraw (*this);
	}

	// swap front and back buffers
	glfwSwapBuffers (window);
	
	// poll input
	glfwPollEvents ();
}

void GLRenderer::OnEnd ()
{
}

void GLRenderer::DrawGameObject (GameObject & gameObject)
{
}

void GLRenderer::DrawGameObject (MeshObject & meshObject)
{
}
