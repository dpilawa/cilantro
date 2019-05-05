#include "cilantroengine.h"
#include "GLFW/glfw3.h"
#include "LogMessage.h"
#include "InputEvent.h"
#include "GLFWInputController.h"

std::unordered_map<InputEventKey, int> GLFWInputController::glfwKeyMap {
        {InputEventKey::KeyA, GLFW_KEY_A},
        {InputEventKey::KeyD, GLFW_KEY_D},
        {InputEventKey::KeyS, GLFW_KEY_S},
        {InputEventKey::KeyW, GLFW_KEY_W},
        {InputEventKey::KeyEsc, GLFW_KEY_ESCAPE},
        {InputEventKey::MouseLeft, GLFW_MOUSE_BUTTON_LEFT},
        {InputEventKey::MouseRight, GLFW_MOUSE_BUTTON_RIGHT}
};

GLFWInputController::GLFWInputController(GLFWwindow** window) : window (window)
{

}

GLFWInputController::~GLFWInputController()
{

}

void GLFWInputController::Initialize () 
{
    glfwSetWindowUserPointer (*window, this);

    auto callback = [](GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
    {
        static_cast<GLFWInputController*>(glfwGetWindowUserPointer (_window))->KeyCallback(_key, _scancode, _action, _mods);
    };

    glfwSetKeyCallback (*window, callback);
    LogMessage (__func__) << "GLFWInputController started";
}

void GLFWInputController::Deinitialize () 
{
    glfwSetKeyCallback (*window, NULL);
}

void GLFWInputController::OnFrame ()
{
    glfwPollEvents ();
    InputController::OnFrame ();
}

InputEvent* GLFWInputController::CreateInputEvent (std::string name, InputEventKey key, InputEventTrigger trigger)
{
    InputEvent* event = InputController::CreateInputEvent (name, key, trigger);

    if (event) 
    {
        auto find = glfwKeyMap.find (key);
        if (find == glfwKeyMap.end())
        {
		    LogMessage (__func__, EXIT_FAILURE) << "Not supported event key for event" << name;
        }
        else 
        {
            glfwEventMap[std::make_tuple(find->second, trigger == InputEventTrigger::Press ? GLFW_PRESS : GLFW_RELEASE, 0)] = event;
        	LogMessage (__func__) << "Mapped key event" << find->second << (trigger == InputEventTrigger::Press ? GLFW_PRESS : GLFW_RELEASE) << 0;

        }
    }

    return event;
}

InputAxis*  GLFWInputController::CreateInputAxis (std::string name, InputAxis axis, float scale) 
{
    return NULL; //fixme
}

void GLFWInputController::KeyCallback (int key, int scancode, int action, int mods)
{
    auto find = glfwEventMap.find (std::make_tuple(key, action, mods));

    if (find == glfwEventMap.end())
    {
	    LogMessage (__func__) << "Unhandled key event" << key << action << mods;
    }
    else
    {
        find->second->Set ();
    }
}