#ifndef _GLFWRENDERER_H_
#define _GLFWRENDERER_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "graphics/GLRenderer.h"

class CGLFWRenderer : public CGLRenderer
{
public:

    __EAPI CGLFWRenderer (GameScene* gameScene, unsigned int width, unsigned int height, bool isDeferred, std::string windowCaption, bool isFullscreen, bool isResizable, bool isVSync);
    __EAPI ~CGLFWRenderer ();

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