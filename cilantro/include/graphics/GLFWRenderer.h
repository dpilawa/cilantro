#ifndef _GLFWRENDERER_H_
#define _GLFWRENDERER_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "graphics/GLRenderer.h"

class CGLFWRenderer : public CGLRenderer
{
public:

    __EAPI CGLFWRenderer (CGameScene* gameScene, unsigned int width, unsigned int height, bool shadowMappingEnabled, bool deferredRenderingEnabled, std::string windowCaption, bool fullscreen, bool resizable, bool vSync);
    __EAPI ~CGLFWRenderer ();

    __EAPI virtual void Initialize ();
    __EAPI virtual void Deinitialize ();

    virtual void RenderFrame ();

private:

    // GL window context
    GLFWwindow* window;

    // parameters
    std::string m_windowCaption;
    bool m_isFullscreen;
    bool m_isResizable;
    bool m_isVSync;

};

#endif