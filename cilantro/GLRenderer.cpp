#include "GLRenderer.h"

GLRenderer::GLRenderer (GameScene& scene, int xRes, int yRes) : Renderer(scene), xResolution (xRes), yResolution (yRes)
{
	glfwInit ();
}


GLRenderer::~GLRenderer ()
{
	glfwTerminate ();
}

void GLRenderer::Initialize ()
{
	// set up GL & window properties
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint (GLFW_RESIZABLE, GL_FALSE);
	
	// create window
	window = glfwCreateWindow (xResolution, yResolution, "GL", nullptr, nullptr);
	if (window == NULL)
	{
		LogMessage (__FUNCTION__) << "GLFW unable to create window";
		std::exit (EXIT_FAILURE);
	}

	// make openGL context active
	glfwMakeContextCurrent (window);

	// set vsync on
	glfwSwapInterval (1);

	// initialize GLEW
	if (glewInit () != GLEW_OK)
	{
		LogMessage (__FUNCTION__) << "GLEW initialization failed";
		std::exit (EXIT_FAILURE);
	}

	// initialize object buffers
	InitializeBuffers ();

	// set callback for new objects
	renderedScene.RegisterCallback (std::bind (&GLRenderer::OnUpdateGameObject, this, std::placeholders::_1));
}

void GLRenderer::RenderFrame ()
{
	// invoke base class function
	Renderer::RenderFrame ();

	// draw scene
	// ...

	// update game clocks (Tock)
	Time::Tock ();

	// swap front and back buffers
	glfwSwapBuffers (window);
	
	// poll input
	glfwPollEvents ();
}

void GLRenderer::Deinitialize ()
{
	buffers.clear ();
	glfwDestroyWindow (window);
}

void GLRenderer::Draw (MeshObject & meshobject)
{
	// Draw mesh
}

void GLRenderer::InitializeBuffers ()
{
	for (GameObject* gameObject : renderedScene.GetGameObjects ())
	{
		unsigned int objectHandle = gameObject->GetHandle ();
		LoadBuffers (objectHandle);
	}
}

void GLRenderer::LoadBuffers (unsigned int objectHandle)
{
	buffers.insert_or_assign (objectHandle, Buffers ());

	// bind Vertex Array Object
	glBindVertexArray (buffers[objectHandle].VAO);
		
	// copy vertices to GPU
	glBindBuffer (GL_ARRAY_BUFFER, buffers[objectHandle].VBO);
	//glBufferData (GL_ARRAY_BUFFER, renderedScene.GetGameObjects()[objectHandle]->, )

}

void GLRenderer::OnUpdateGameObject (unsigned int objectHandle)
{
	LogMessage (__FUNCTION__) << objectHandle;

	// update object buffers
	LoadBuffers (objectHandle);
}


