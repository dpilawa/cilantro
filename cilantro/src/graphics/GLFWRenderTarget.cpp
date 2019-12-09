#include "cilantroengine.h"
#include "graphics/RenderTarget.h"
#include "graphics/GLFWRenderTarget.h"
#include "util/LogMessage.h"
#include "util/Timer.h"
#include "math/Vector3f.h"
#include "scene/GameScene.h"

GLFWRenderTarget::GLFWRenderTarget (GameLoop* gameLoop, std::string windowCaption, unsigned int width, unsigned int height, bool isFullscreen, bool isResizable, bool isVSync) : RenderTarget (gameLoop, width, height)
{
	this->windowCaption = windowCaption;
	this->isFullscreen = isFullscreen;
	this->isResizable = isResizable;
	this->isVSync = isVSync;

	// initialize
	glfwInit ();
	this->Initialize ();
}

GLFWRenderTarget::~GLFWRenderTarget ()
{
	this->Deinitialize ();
	glfwTerminate ();
}

void GLFWRenderTarget::OnFrame ()
{
	glRenderer = dynamic_cast<GLRenderer*>(gameLoop->gameRenderer);

	// draw quad on screen
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable (GL_DEPTH_TEST);
	glRenderer->GetShaderProgram ("flatquad_shader").Use ();
	glBindVertexArray (targetVAO);
	glBindTexture (GL_TEXTURE_2D, glRenderer->GetFrameBufferTexture ());
	glViewport (0, 0, this->width, this->height);
	glDrawArrays (GL_TRIANGLES, 0, 6);

	// swap front and back buffers
	glfwSwapBuffers (window);
}

GLFWwindow* GLFWRenderTarget::GetWindow ()
{
	return window;
}

void GLFWRenderTarget::Initialize ()
{
	GLFWmonitor* monitor; 

	// check fullscreen
	if (isFullscreen)
	{
		monitor = glfwGetPrimaryMonitor ();

		width = glfwGetVideoMode (monitor)->width;
		height = glfwGetVideoMode (monitor)->height;
	}
	else
	{
		monitor = nullptr;
	}

	// set up GL & window properties
	glfwWindowHint (GLFW_RESIZABLE, isResizable);
	glfwWindowHint (GLFW_VISIBLE, 1);

	// create window
	window = glfwCreateWindow (width, height, windowCaption.c_str (), monitor, nullptr);

	if (window == NULL)
	{
		LogMessage (__func__, EXIT_FAILURE) << "GLFW unable to create window";
	}

	// set resize callback
	glfwSetWindowUserPointer (window, reinterpret_cast<void *>(this));

	auto framebufferResizeCallback = [](GLFWwindow* window, int width, int height)
    {
        reinterpret_cast<GLFWRenderTarget*>(glfwGetWindowUserPointer (window))->FramebufferResizeCallback (width, height);
    };

	glfwSetFramebufferSizeCallback (window, framebufferResizeCallback);

	// make openGL context active
	glfwMakeContextCurrent (window);

	// set vsync on
	glfwSwapInterval (isVSync);

	// load GL
    if (!gladLoadGL ())
    {
        LogMessage (__func__, EXIT_FAILURE) << "GL context initialization failed";
    }

	// set-up VAO and VBO for onscreen rendering
 	float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

	glGenVertexArrays(1, &targetVAO);
    glGenBuffers(1, &targetVBO);
    glBindVertexArray(targetVAO);
    glBindBuffer(GL_ARRAY_BUFFER, targetVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof (quadVertices), &quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof (float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	LogMessage (__func__) << "GLFWRenderTarget started";
}

void GLFWRenderTarget::Deinitialize ()
{
	glfwDestroyWindow (window);
}

void GLFWRenderTarget::FramebufferResizeCallback (int width, int height)
{
	this->width = width;
	this->height = height;

	// update GL renderer texture size and viewport
	glRenderer->SetResolution (width, height);
}

