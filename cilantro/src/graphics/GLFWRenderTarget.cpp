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
    glDisable (GL_FRAMEBUFFER_SRGB);
    glRenderer->GetShaderProgram ("flatquad_shader").Use ();
    glBindVertexArray (targetVAO);
    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D, glRenderer->GetRendererFramebufferTexture ());
    glViewport (0, 0, this->width, this->height);
    glDrawArrays (GL_TRIANGLES, 0, 6);
    glBindVertexArray (0);
    glBindTexture (GL_TEXTURE_2D, 0);

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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, CILANTRO_GL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, CILANTRO_GL_VERSION_MINOR);
    glfwWindowHint(GLFW_RESIZABLE, isResizable);
    glfwWindowHint(GLFW_VISIBLE, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, 0);

    // create window
    window = glfwCreateWindow (width, height, windowCaption.c_str (), monitor, nullptr);

    if (window == NULL)
    {
        LogMessage (__func__, EXIT_FAILURE) << "GLFW unable to create window";
    }

    // make openGL context active
    glfwMakeContextCurrent (window);

    // set resize callback
    glfwSetWindowUserPointer (window, this->gameLoop);

    auto framebufferResizeCallback = [](GLFWwindow* window, int width, int height)
    {
        static_cast<GLFWRenderTarget*>(static_cast<GameLoop*>(glfwGetWindowUserPointer (window))->gameRenderTarget)->FramebufferResizeCallback (width, height);
    };

    glfwSetFramebufferSizeCallback (window, framebufferResizeCallback);

    // set framebuffer size (relevant for high DPI displays)
    // glfwGetFramebufferSize (window, (int*)&width, (int*)&height);

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

