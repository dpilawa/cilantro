#ifndef _GLFWRENDERER_H_
#define _GLFWRENDERER_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

class Renderer;

class GLFWRenderer : public Renderer
{
public:

    __EAPI GLFWRenderer (unsigned int width, unsigned int height, std::string windowCaption, bool isFullscreen, bool isResizable, bool isVSync);
    __EAPI ~GLFWRenderer ();

    virtual void Initialize ();
    virtual void Deinitialize ();

    virtual void RenderFrame ();

private:

    // GL window context
    GLFWwindow* window;

    // parameters
    std::string windowCaption;
    bool isFullscreen;
    bool isResizable;
    bool isVSync;

};

#endif