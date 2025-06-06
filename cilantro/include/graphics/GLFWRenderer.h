#ifndef _GLFWRENDERER_H_
#define _GLFWRENDERER_H_

#include "cilantroengine.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "graphics/GLRenderer.h"

namespace cilantro {

class __CEAPI GLFWRenderer : public GLRenderer
{
public:

    __EAPI GLFWRenderer (std::shared_ptr<GameScene> gameScene, unsigned int width, unsigned int height, bool shadowMappingEnabled, bool deferredRenderingEnabled, std::string windowCaption, bool fullscreen, bool resizable, bool vSync);
    __EAPI ~GLFWRenderer ();

    __EAPI virtual void Initialize ();
    __EAPI virtual void Deinitialize ();

    __EAPI virtual void RenderFrame ();

private:

    void DetectGLVersion ();

    static void FramebufferResizeCallback (GLFWwindow* window, int width, int height);

    // GL window context
    GLFWwindow* window;

    // parameters
    std::string m_windowCaption;
    bool m_isFullscreen;
    bool m_isResizable;
    bool m_isVSync;

};

} // namespace cilantro

#endif