#include "GLFWRenderTarget.h"

#include <imgui.h>
#include "examples/opengl3_example/imgui_impl_glfw_gl3.h"

GLFWRenderTarget::GLFWRenderTarget (int xRes, int yRes, int fullscreen, int vsync) : RenderTarget (xRes, yRes)
{
	isFullscreen = fullscreen;
	vSync = vsync;

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
	// set up GL & window properties
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint (GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint (GLFW_SAMPLES, 4);
	glfwWindowHint (GLFW_VISIBLE, 1);

	// create window
	window = glfwCreateWindow (xResolution, yResolution, "GL", nullptr, nullptr);
	if (window == NULL)
	{
		LogMessage (__FUNCTION__, EXIT_FAILURE) << "GLFW unable to create window";
	}

	// make openGL context active
	glfwMakeContextCurrent (window);

	// set vsync on
	glfwSwapInterval (vSync);

	// initialize GLEW
	if (glewInit () != GLEW_OK)
	{
		LogMessage (__FUNCTION__, EXIT_FAILURE) << "GLEW initialization failed";
	}

	// Setup ImGui binding
	ImGui_ImplGlfwGL3_Init (window, true);
	ImGui::StyleColorsClassic ();
}

void GLFWRenderTarget::Deinitialize ()
{
	ImGui_ImplGlfwGL3_Shutdown ();
	glfwDestroyWindow (window);
}

void GLFWRenderTarget::BeforeFrame ()
{
	// tick imgui
	ImGui_ImplGlfwGL3_NewFrame ();
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

	// swap front and back buffers
	glfwSwapBuffers (window);

	// poll input
	glfwPollEvents ();
}
