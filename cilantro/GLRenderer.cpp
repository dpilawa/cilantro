#include "GLRenderer.h"
#include "LogMessage.h"

GLRenderer::GLRenderer (GameScene& scene, int xRes, int yRes) : Renderer(scene), xResolution (xRes), yResolution (yRes)
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
	window = glfwCreateWindow (xResolution, yResolution, "GL", nullptr, nullptr);

	// make openGL context active
	glfwMakeContextCurrent (window);

	// set vsync on
	glfwSwapInterval (1);

	// set callback for new objects
	renderedScene.RegisterGameObjectModifiedCallback (std::bind (&GLRenderer::OnUpdateGameObject, this, std::placeholders::_1));
}

void GLRenderer::OnFrame ()
{
	// draw scene


	// swap front and back buffers
	glfwSwapBuffers (window);
	
	// poll input
	glfwPollEvents ();
}

void GLRenderer::OnEnd ()
{
}

void GLRenderer::OnUpdateGameObject (size_t objectHandle)
{
	LogMessage () << "GLRenderer received callback on new or updated object [" << objectHandle << "]";
}


