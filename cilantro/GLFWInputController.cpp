#include "cilantroengine.h"
#include "GLFW/glfw3.h"
#include "LogMessage.h"
#include "InputEvent.h"
#include "GLFWInputController.h"

std::unordered_map<InputKey, int, InputKeyHash> GLFWInputController::glfwKeyMap {
        {InputKey::KeyA, GLFW_KEY_A},
        {InputKey::KeyB, GLFW_KEY_B},
        {InputKey::KeyC, GLFW_KEY_C},
        {InputKey::KeyD, GLFW_KEY_D},
        {InputKey::KeyE, GLFW_KEY_E},
        {InputKey::KeyF, GLFW_KEY_F},
        {InputKey::KeyG, GLFW_KEY_G},
        {InputKey::KeyH, GLFW_KEY_H},
        {InputKey::KeyI, GLFW_KEY_I},
        {InputKey::KeyJ, GLFW_KEY_J},
        {InputKey::KeyK, GLFW_KEY_K},
        {InputKey::KeyL, GLFW_KEY_L},
        {InputKey::KeyM, GLFW_KEY_M},
        {InputKey::KeyN, GLFW_KEY_N},
        {InputKey::KeyO, GLFW_KEY_O},
        {InputKey::KeyP, GLFW_KEY_P},
        {InputKey::KeyQ, GLFW_KEY_Q},
        {InputKey::KeyR, GLFW_KEY_R},
        {InputKey::KeyS, GLFW_KEY_S},
        {InputKey::KeyT, GLFW_KEY_T},
        {InputKey::KeyU, GLFW_KEY_U},
        {InputKey::KeyV, GLFW_KEY_V},
        {InputKey::KeyW, GLFW_KEY_W},
        {InputKey::KeyX, GLFW_KEY_X},
        {InputKey::KeyY, GLFW_KEY_Y},
        {InputKey::KeyZ, GLFW_KEY_Z},
        {InputKey::Key1, GLFW_KEY_1},
        {InputKey::Key2, GLFW_KEY_2},
        {InputKey::Key3, GLFW_KEY_3},
        {InputKey::Key4, GLFW_KEY_4},
        {InputKey::Key5, GLFW_KEY_5},
        {InputKey::Key6, GLFW_KEY_6},
        {InputKey::Key7, GLFW_KEY_7},
        {InputKey::Key8, GLFW_KEY_8},
        {InputKey::Key9, GLFW_KEY_9},
        {InputKey::Key0,  GLFW_KEY_0},
        {InputKey::KeyUp, GLFW_KEY_UP},
        {InputKey::KeyDown, GLFW_KEY_DOWN},
        {InputKey::KeyLeft, GLFW_KEY_LEFT},
        {InputKey::KeyRight, GLFW_KEY_RIGHT},
        {InputKey::KeyEsc, GLFW_KEY_ESCAPE},
        {InputKey::KeyEnter, GLFW_KEY_ENTER},
        {InputKey::KeySpace, GLFW_KEY_SPACE},
        {InputKey::KeyLeftControl, GLFW_KEY_LEFT_CONTROL},
        {InputKey::KeyRightControl, GLFW_KEY_RIGHT_CONTROL},
        {InputKey::KeyLeftShift, GLFW_KEY_LEFT_SHIFT},
        {InputKey::KeyRightShift, GLFW_KEY_RIGHT_SHIFT},
        {InputKey::KeyLeftAlt, GLFW_KEY_LEFT_ALT},
        {InputKey::KeyRightAlt,  GLFW_KEY_RIGHT_ALT},
        {InputKey::MouseLeft, GLFW_MOUSE_BUTTON_LEFT},
        {InputKey::MouseMiddle, GLFW_MOUSE_BUTTON_MIDDLE},
        {InputKey::MouseRight, GLFW_MOUSE_BUTTON_RIGHT}
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

    auto keyCallback = [](GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
    {
        static_cast<GLFWInputController*>(glfwGetWindowUserPointer (_window))->KeyCallback(_key, _scancode, _action, _mods);
    };

    auto mouseButtonCallback = [](GLFWwindow* _window, int _button, int _action, int _mods)
    {
        static_cast<GLFWInputController*>(glfwGetWindowUserPointer (_window))->KeyCallback(_button, 0, _action, _mods);
    };

    auto mouseCursorCallback = [](GLFWwindow* _window, double _xpos, double _ypos)
    {
        static_cast<GLFWInputController*>(glfwGetWindowUserPointer (_window))->MouseCursorCallback(_xpos, _ypos);
    };


    glfwSetKeyCallback (*window, keyCallback);
    glfwSetMouseButtonCallback (*window, mouseButtonCallback);
    glfwSetCursorPosCallback (*window, mouseCursorCallback);
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

InputEvent* GLFWInputController::CreateInputEvent (std::string name, InputKey key, InputTrigger trigger, std::set<InputModifier> modifiers)
{
    InputEvent* event = InputController::CreateInputEvent (name, key, trigger, modifiers);

    if (event) 
    {
        auto find = glfwKeyMap.find (key);
        if (find == glfwKeyMap.end())
        {
		    LogMessage (__func__, EXIT_FAILURE) << "Not supported event key for event" << name;
        }
        else 
        {
            int mods = 0;

            for (auto& m : modifiers) 
            {
                switch (m) 
                {
                    case InputModifier::Alt : mods |= GLFW_MOD_ALT;
                        break;
                    case InputModifier::Control : mods |= GLFW_MOD_CONTROL;
                        break;
                    case InputModifier::Shift : mods |= GLFW_MOD_SHIFT;
                        break;
                    default :
                        break;
                }
            }

            glfwEventMap[std::make_tuple (find->second, trigger == InputTrigger::Press ? GLFW_PRESS : GLFW_RELEASE, mods)] = event;
            LogMessage (__func__) << "Mapped key event" << find->second << (trigger == InputTrigger::Press ? GLFW_PRESS : GLFW_RELEASE) << mods;
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
	    LogMessage (__func__) << "Unhandled key/button event" << key << action << mods;
    }
    else
    {
        find->second->Set ();
    }
}

void GLFWInputController::MouseCursorCallback(double xpos, double ypos)
{
    LogMessage (__func__) << "Unhandled mouse cursor event" << xpos << ypos;    
}