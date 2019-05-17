#include "cilantroengine.h"
#include "GLFW/glfw3.h"
#include "LogMessage.h"
#include "InputEvent.h"
#include "GLFWInputController.h"

std::unordered_map<InputEventKey, int, InputEventKeyHash> GLFWInputController::glfwKeyMap {
        {InputEventKey::KeyA, GLFW_KEY_A},
        {InputEventKey::KeyB, GLFW_KEY_B},
        {InputEventKey::KeyC, GLFW_KEY_C},
        {InputEventKey::KeyD, GLFW_KEY_D},
        {InputEventKey::KeyE, GLFW_KEY_E},
        {InputEventKey::KeyF, GLFW_KEY_F},
        {InputEventKey::KeyG, GLFW_KEY_G},
        {InputEventKey::KeyH, GLFW_KEY_H},
        {InputEventKey::KeyI, GLFW_KEY_I},
        {InputEventKey::KeyJ, GLFW_KEY_J},
        {InputEventKey::KeyK, GLFW_KEY_K},
        {InputEventKey::KeyL, GLFW_KEY_L},
        {InputEventKey::KeyM, GLFW_KEY_M},
        {InputEventKey::KeyN, GLFW_KEY_N},
        {InputEventKey::KeyO, GLFW_KEY_O},
        {InputEventKey::KeyP, GLFW_KEY_P},
        {InputEventKey::KeyQ, GLFW_KEY_Q},
        {InputEventKey::KeyR, GLFW_KEY_R},
        {InputEventKey::KeyS, GLFW_KEY_S},
        {InputEventKey::KeyT, GLFW_KEY_T},
        {InputEventKey::KeyU, GLFW_KEY_U},
        {InputEventKey::KeyV, GLFW_KEY_V},
        {InputEventKey::KeyW, GLFW_KEY_W},
        {InputEventKey::KeyX, GLFW_KEY_X},
        {InputEventKey::KeyY, GLFW_KEY_Y},
        {InputEventKey::KeyZ, GLFW_KEY_Z},
        {InputEventKey::Key1, GLFW_KEY_1},
        {InputEventKey::Key2, GLFW_KEY_2},
        {InputEventKey::Key3, GLFW_KEY_3},
        {InputEventKey::Key4, GLFW_KEY_4},
        {InputEventKey::Key5, GLFW_KEY_5},
        {InputEventKey::Key6, GLFW_KEY_6},
        {InputEventKey::Key7, GLFW_KEY_7},
        {InputEventKey::Key8, GLFW_KEY_8},
        {InputEventKey::Key9, GLFW_KEY_9},
        {InputEventKey::Key0,  GLFW_KEY_0},
        {InputEventKey::KeyUp, GLFW_KEY_UP},
        {InputEventKey::KeyDown, GLFW_KEY_DOWN},
        {InputEventKey::KeyLeft, GLFW_KEY_LEFT},
        {InputEventKey::KeyRight, GLFW_KEY_RIGHT},
        {InputEventKey::KeyEsc, GLFW_KEY_ESCAPE},
        {InputEventKey::KeyEnter, GLFW_KEY_ENTER},
        {InputEventKey::KeySpace, GLFW_KEY_SPACE},
        {InputEventKey::KeyLeftControl, GLFW_KEY_LEFT_CONTROL},
        {InputEventKey::KeyRightControl, GLFW_KEY_RIGHT_CONTROL},
        {InputEventKey::KeyLeftShift, GLFW_KEY_LEFT_SHIFT},
        {InputEventKey::KeyRightShift, GLFW_KEY_RIGHT_SHIFT},
        {InputEventKey::KeyLeftAlt, GLFW_KEY_LEFT_ALT},
        {InputEventKey::KeyRightAlt,  GLFW_KEY_RIGHT_ALT},
        {InputEventKey::MouseLeft, GLFW_MOUSE_BUTTON_LEFT},
        {InputEventKey::MouseMiddle, GLFW_MOUSE_BUTTON_MIDDLE},
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

InputEvent* GLFWInputController::CreateInputEvent (std::string name, InputEventKey key, InputEventTrigger trigger, std::set<InputEventModifier> modifiers, float multiplier)
{
    InputEvent* event = InputController::CreateInputEvent (name, key, trigger, modifiers, multiplier);

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
        LogMessage (__func__) << "Set event" << key << find->second->GetName ();
        find->second->Set ();
    }
}