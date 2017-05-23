#include "GLRenderer.h"

GLRenderer::GLRenderer ()
{
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
	window = glfwCreateWindow (800, 450, "GL", nullptr, nullptr);

	// make openGL context active
	glfwMakeContextCurrent (window);

	// set vsync on
	glfwSwapInterval (1);
}

void GLRenderer::OnFrame ()
{
	glfwSwapBuffers (window);
	glfwPollEvents ();
}

void GLRenderer::OnEnd ()
{
}
