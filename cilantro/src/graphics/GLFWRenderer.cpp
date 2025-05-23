#include "cilantroengine.h"
#include "graphics/GLRenderer.h"
#include "graphics/GLFWRenderer.h"
#include "scene/GameScene.h"
#include "system/Game.h"
#include "system/LogMessage.h"

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
    GLint data;
    
    // initialize GLFW
    glfwInit ();

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
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, CILANTRO_GLES_VERSION_MAJOR);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, CILANTRO_GLES_VERSION_MINOR);
#else
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, CILANTRO_GL_VERSION_MAJOR);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, CILANTRO_GL_VERSION_MINOR);
#endif    
    glfwWindowHint (GLFW_RESIZABLE, m_isResizable);
    glfwWindowHint (GLFW_VISIBLE, 1);
#if (CILANTRO_GL_VERSION < 150)
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
#else
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
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
    // display GL version information
    LogMessage (MSG_LOCATION) << "Version:" << (char*) glGetString (GL_VERSION);
    LogMessage (MSG_LOCATION) << "Shader language version:" << (char*) glGetString (GL_SHADING_LANGUAGE_VERSION);
    LogMessage (MSG_LOCATION) << "Renderer:" << (char*) glGetString (GL_RENDERER);

    glGetIntegerv (GL_MAX_GEOMETRY_SHADER_INVOCATIONS, &data);
    if (glGetError() == GL_NO_ERROR)
    {
        LogMessage (MSG_LOCATION) << "GL_MAX_GEOMETRY_SHADER_INVOCATIONS =" << std::to_string (data);
    }

    glGetIntegerv (GL_MAX_ARRAY_TEXTURE_LAYERS, &data);
    if (glGetError() == GL_NO_ERROR)
    {
        LogMessage (MSG_LOCATION) << "GL_MAX_ARRAY_TEXTURE_LAYERS =" << std::to_string (data);
    }

    glGetIntegerv (GL_MAX_VERTEX_OUTPUT_COMPONENTS, &data);
    if (glGetError() == GL_NO_ERROR)
    {
        LogMessage (MSG_LOCATION) << "GL_MAX_VERTEX_OUTPUT_COMPONENTS =" << std::to_string (data);
    }

    glGetIntegerv (GL_MAX_FRAMEBUFFER_LAYERS, &data);
    if (glGetError() == GL_NO_ERROR)
    {
        LogMessage (MSG_LOCATION) << "GL_MAX_FRAMEBUFFER_LAYERS =" << std::to_string (data); 
    }

    glGetIntegerv (GL_MAX_FRAMEBUFFER_WIDTH, &data);
    if (glGetError() == GL_NO_ERROR)
    {
        LogMessage (MSG_LOCATION) << "GL_MAX_FRAMEBUFFER_WIDTH =" << std::to_string (data); 
    }

    glGetIntegerv (GL_MAX_FRAMEBUFFER_HEIGHT, &data);
    if (glGetError() == GL_NO_ERROR)
    {
        LogMessage (MSG_LOCATION) << "GL_MAX_FRAMEBUFFER_HEIGHT =" << std::to_string (data); 
    }

    glGetIntegerv (GL_MAX_UNIFORM_BLOCK_SIZE, &data);
    if (glGetError() == GL_NO_ERROR)
    {
        LogMessage (MSG_LOCATION) << "GL_MAX_UNIFORM_BLOCK_SIZE =" << std::to_string (data); 
    }

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

void GLFWRenderer::FramebufferResizeCallback (GLFWwindow* window, int width, int height)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer (window));
    for (auto&& gameScene : game->GetGameSceneManager ())
    {
        gameScene->GetRenderer ()->SetResolution (width, height);
    }
};

} // namespace cilantro
