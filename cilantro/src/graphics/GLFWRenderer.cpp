#include "cilantroengine.h"
#include "graphics/Renderer.h"
#include "graphics/GLFWRenderer.h"
#include "system/Game.h"
#include "system/LogMessage.h"

GLFWRenderer::GLFWRenderer (unsigned int width, unsigned int height, std::string windowCaption, bool isFullscreen, bool isResizable, bool isVSync) : Renderer (width, height)
{
    this->windowCaption = windowCaption;
    this->isFullscreen = isFullscreen;
    this->isResizable = isResizable;
    this->isVSync = isVSync;
}

GLFWRenderer::~GLFWRenderer ()
{

}

void GLFWRenderer::Initialize ()
{
    GLFWmonitor* monitor; 
    
    // initialize GLFW
    glfwInit ();

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
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, CILANTRO_GL_VERSION_MAJOR);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, CILANTRO_GL_VERSION_MINOR);
    glfwWindowHint (GLFW_RESIZABLE, isResizable);
    glfwWindowHint (GLFW_VISIBLE, 1);
#if (CILANTRO_GL_VERSION < 150)
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
#else
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_COCOA_RETINA_FRAMEBUFFER, GL_TRUE);

    // create window
    window = glfwCreateWindow (width, height, windowCaption.c_str (), monitor, nullptr);

    if (window == NULL)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "GLFW unable to create window";
    }

    // make openGL context active
    glfwMakeContextCurrent (window);

    // set resize callback
    auto framebufferResizeCallback = [](GLFWwindow* window, int width, int height)
    {
        Game::GetRenderer().SetResolution (width, height);
    };

    glfwSetFramebufferSizeCallback (window, framebufferResizeCallback);

    // set framebuffer size (relevant for high DPI displays)
    glfwGetFramebufferSize (window, (int*)(&width), (int*)(&height));

    // set vsync on
    glfwSwapInterval (isVSync);

    // load GL
    if (!gladLoadGL ())
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "GL context initialization failed";
    }

    // display GL version information
    LogMessage (MSG_LOCATION) << "Version:" << (char*) glGetString (GL_VERSION);
    LogMessage (MSG_LOCATION) << "Shader language version:" << (char*) glGetString (GL_SHADING_LANGUAGE_VERSION);
    LogMessage (MSG_LOCATION) << "Renderer:" << (char*) glGetString (GL_RENDERER);

    LogMessage (MSG_LOCATION) << "GLFWRenderer started";

    Renderer::Initialize ();
}

void GLFWRenderer::Deinitialize ()
{    
    Renderer::Deinitialize ();
    
    glfwDestroyWindow (window);
    glfwTerminate ();
}

void GLFWRenderer::RenderFrame ()
{
    Renderer::RenderFrame ();

    // swap front and back buffers
    glfwSwapBuffers (window);

    // check window closing
    if (glfwWindowShouldClose (window))
    {
        Game::Stop ();
    }
}
