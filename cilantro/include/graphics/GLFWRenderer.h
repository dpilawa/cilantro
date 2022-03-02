#ifndef _GLFWRENDERER_H_
#define _GLFWRENDERER_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "graphics/GLRenderer.h"

class GLFWRenderer : public GLRenderer
{
public:

    __EAPI GLFWRenderer (GameScene* gameScene, unsigned int width, unsigned int height, bool isDeferred, std::string windowCaption, bool isFullscreen, bool isResizable, bool isVSync);
    __EAPI ~GLFWRenderer ();

    __EAPI virtual void Initialize ();
    __EAPI virtual void Deinitialize ();

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