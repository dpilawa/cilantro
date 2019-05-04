#include "cilantroengine.h"
#include "GL/gl3w.h"
#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"
#include "RenderTarget.h"
#include "GLFWRenderTarget.h"
#include "LogMessage.h"
#include "Time.h"

GLFWRenderTarget::GLFWRenderTarget (int xRes, int yRes) : RenderTarget (xRes, yRes)
{
	isFullscreen = false;
	isResizable = false;
	isVSync = true;
	isDebugVisible = true;

	glfwInit ();

	frameCount = 0;
	lastFrameCount = 0;
	splitFrameCount = 0;
}

GLFWRenderTarget::~GLFWRenderTarget ()
{
	glfwTerminate ();
}

void GLFWRenderTarget::Initialize ()
{
    // create temporary window to detect GL version
	glfwWindowHint (GLFW_VISIBLE, 0);
    window = glfwCreateWindow (xResolution, yResolution, "GL", nullptr, nullptr);
	if (window == NULL)
	{
		LogMessage (__func__, EXIT_FAILURE) << "GLFW unable to create window";
	}
	glfwMakeContextCurrent (window);

	// initialize GLEW
	if (gl3wInit ())
	{
		LogMessage (__func__, EXIT_FAILURE) << "GLEW initialization failed";
	}

	// display GL version information
	LogMessage (__func__) << "Version:" << (char*) glGetString (GL_VERSION);
    LogMessage (__func__) << "Shader language version:" << (char*) glGetString (GL_SHADING_LANGUAGE_VERSION);
	LogMessage (__func__) << "Renderer:" << (char*) glGetString (GL_RENDERER);
	glfwDestroyWindow (window);

	// set up GL & window properties
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
	glfwWindowHint (GLFW_RESIZABLE, isResizable);
	glfwWindowHint (GLFW_SAMPLES, 4);
	glfwWindowHint (GLFW_VISIBLE, 1);

	// create window
	window = glfwCreateWindow (xResolution, yResolution, "GL", nullptr, nullptr);
	if (window == NULL)
	{
		LogMessage (__func__, EXIT_FAILURE) << "GLFW unable to create window";
	}

	// make openGL context active
	glfwMakeContextCurrent (window);

	// set vsync on
	glfwSwapInterval (isVSync);

	// Setup ImGui binding
	const char* glsl_version = "#version 140";
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL (window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

	LogMessage (__func__) << "GLFWRenderTarget started";
}

void GLFWRenderTarget::Deinitialize ()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	glfwDestroyWindow (window);
}

void GLFWRenderTarget::BeforeFrame ()
{

	// resize viewport (window size may have changed)
	glfwGetFramebufferSize (window, &xResolution, &yResolution);
	glViewport (0, 0, xResolution, yResolution);

	// tick imgui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GLFWRenderTarget::AfterFrame ()
{
	frameCount++;

	// display debug messages

	if (Time::GetTimeSinceSplitTime () > 1.0f)
	{
		timeSinceLastSplit = Time::GetTimeSinceSplitTime ();
		Time::ResetSplitTime ();
		splitFrameCount = frameCount - lastFrameCount;
		lastFrameCount = frameCount;
		frameRenderTimeInLastSplit = frameRenderTimeSinceLastSplit;
		frameRenderTimeSinceLastSplit = 0;
	}

	frameRenderTimeSinceLastSplit += Time::GetFrameRenderTime ();


	ImGui::Text ("FPS: %.1f", splitFrameCount / timeSinceLastSplit);
	ImGui::Text ("Frame time: %.1f ms", frameRenderTimeInLastSplit / splitFrameCount * 1000.0f);

	// render imgui
	ImGui::Render ();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// swap front and back buffers
	glfwSwapBuffers (window);

	// poll input
	glfwPollEvents ();
}

GLFWwindow** GLFWRenderTarget::GetWindow ()
{
	return &window;
}

void GLFWRenderTarget::SetFullscreen (bool fullscreen)
{
	isFullscreen = fullscreen;
}

void GLFWRenderTarget::SetResizable (bool resizable)
{
	isResizable = resizable;
}

void GLFWRenderTarget::SetVSync (bool vsync)
{
	isVSync = vsync;
}

void GLFWRenderTarget::SetDebugVisible (bool debugvisible)
{
	isDebugVisible = debugvisible;
}
