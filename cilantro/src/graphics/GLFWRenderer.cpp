#include "cilantroengine.h"
#include "graphics/GLRenderer.h"
#include "graphics/GLFWRenderer.h"
#include "scene/GameScene.h"
#include "system/Game.h"
#include "system/LogMessage.h"
#include <vector>
#include <tuple>

namespace cilantro {

GLFWRenderer::GLFWRenderer (std::shared_ptr<GameScene> gameScene, unsigned int width, unsigned int height, bool shadowMappingEnabled, bool deferredRenderingEnabled, std::string windowCaption, bool fullscreen, bool resizable, bool vSync) 
    : GLRenderer (gameScene, width, height, shadowMappingEnabled, deferredRenderingEnabled)
    , m_windowCaption (windowCaption)
    , m_isFullscreen (fullscreen)
    , m_isResizable (resizable)
    , m_isVSync (vSync)
{
}

GLFWRenderer::~GLFWRenderer ()
{

}

void GLFWRenderer::Initialize ()
{
    GLFWmonitor* monitor;
    
    // initialize GLFW
    glfwInit ();
    DetectGLVersion ();

    // check fullscreen
    if (m_isFullscreen)
    {
        monitor = glfwGetPrimaryMonitor ();

        m_width = glfwGetVideoMode (monitor)->width;
        m_height = glfwGetVideoMode (monitor)->height;
    }
    else
    {
        monitor = nullptr;
    }

    // set up GL & window properties
#ifdef CILANTRO_BUILDING_GLES
    glfwWindowHint (GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#endif    
    glfwWindowHint (GLFW_RESIZABLE, m_isResizable);
    glfwWindowHint (GLFW_VISIBLE, 1);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_COCOA_RETINA_FRAMEBUFFER, GL_TRUE);

    // create window
    window = glfwCreateWindow (static_cast<int> (m_width), static_cast<int> (m_height), m_windowCaption.c_str (), monitor, nullptr);

    if (window == NULL)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "GLFW unable to create window";
    }

    // make openGL context active
    glfwMakeContextCurrent (window);

    // set game pointer on window (for callbacks)
    glfwSetWindowUserPointer (window, GetGameScene ()->GetGame ().get ());

    // set resize callback
    glfwSetFramebufferSizeCallback (window, FramebufferResizeCallback);

    // set framebuffer size (relevant for high DPI displays)
    glfwGetFramebufferSize (window, (int*)(&m_width), (int*)(&m_height));

    // set vsync on
    glfwSwapInterval (m_isVSync);

    // load GL
#ifdef CILANTRO_BUILD_GLES
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
#else
    if (!gladLoadGL (glfwGetProcAddress))
#endif    
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "GL context initialization failed";
    }

    glGetError(	);

    LogMessage (MSG_LOCATION) << "GLFWRenderer started";

    GLRenderer::Initialize ();
}

void GLFWRenderer::Deinitialize ()
{    
    GLRenderer::Deinitialize ();
    
    glfwDestroyWindow (window);
    glfwTerminate ();
}

void GLFWRenderer::RenderFrame ()
{
    GLRenderer::RenderFrame ();

    // swap front and back buffers
    glfwSwapBuffers (window);

    // check window closing
    if (glfwWindowShouldClose (window))
    {
        GetGameScene ()->GetGame ()->Stop ();
    }
}

void GLFWRenderer::DetectGLVersion ()
{
    std::vector<std::pair<int,int>> candidates = {
        {4,6},{4,5},{4,4},{4,3},{4,2},{4,1},{4,0},
        {3,3},{3,2},{3,1},{3,0},
        {2,1},{2,0}
    };

    for (auto [maj, min] : candidates) {
        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, maj);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, min);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        GLFWwindow* tmp = glfwCreateWindow(1,1,"", nullptr, nullptr);
        if (tmp) {
            std::string version = std::to_string(maj) + "." + std::to_string(min);
            LogMessage(MSG_LOCATION) << "OpenGL version found:" << version;
            glfwDestroyWindow(tmp);
            break;
        }
    }
}

void GLFWRenderer::FramebufferResizeCallback (GLFWwindow* window, int width, int height)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer (window));
    for (auto&& gameScene : game->GetGameSceneManager ())
    {
        gameScene->GetRenderer ()->SetResolution (width, height);
    }
};

} // namespace cilantro
