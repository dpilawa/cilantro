#include "cilantroengine.h"
#include "input/GLFWInputController.h"
#include "system/EngineContext.h"
#include "system/LogMessage.h"
#include "input/Input.h"

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

GLFWInputController::GLFWInputController () : InputController ()
{
    axisMouseX = nullptr;
    axisMouseY = nullptr;
    axisMouseScrollX = nullptr;
    axisMouseScrollY = nullptr;

    isGameMode = false;
}

GLFWInputController::~GLFWInputController ()
{

}

void GLFWInputController::OnFrame ()
{
    glfwPollEvents ();
    InputController::OnFrame ();
}

Input<bool>* GLFWInputController::CreateInputEvent (const std::string& name, InputKey key, InputTrigger trigger, std::set<InputModifier> modifiers)
{
    Input<bool>* inputevent = InputController::CreateInputEvent (name);

    if (inputevent) 
    {
        glfwKeyEventMap[std::make_tuple (GetGLFWKey (key), GetGLFWTrigger (trigger), GetGLFWModifiers (modifiers))] = inputevent;
        LogMessage (MSG_LOCATION) << "Mapped key event" << GetGLFWKey (key) << GetGLFWTrigger (trigger) << GetGLFWModifiers (modifiers);
    }

    return inputevent;
}


Input<float>* GLFWInputController::CreateInputAxis (const std::string& name, InputKey key, std::set<InputModifier> modifiers, float scale) 
{
    Input<float>* inputaxis = InputController::CreateInputAxis (name, scale);

    if (inputaxis) 
    {
        glfwKeyAxisMap[std::make_pair (GetGLFWKey (key), GetGLFWModifiers (modifiers))] = inputaxis;
        LogMessage (MSG_LOCATION) << "Mapped key axis" << GetGLFWKey (key) << GetGLFWModifiers (modifiers);
    }

    return inputaxis;
}

Input<float>* GLFWInputController::CreateInputAxis (const std::string& name, InputAxis axis, float scale) 
{
    Input<float>* inputaxis = InputController::CreateInputAxis (name, scale);

    if (inputaxis)
    {
        switch (axis)
        {
            case InputAxis::MouseX :
                axisMouseX = inputaxis;
                break;

            case InputAxis::MouseY :
                axisMouseY = inputaxis;
                break;

            case InputAxis::MouseScrollX :
                axisMouseScrollX = inputaxis;
                break;

            case InputAxis::MouseScrollY :
                axisMouseScrollY = inputaxis;
                break;

            default :
                LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unsupported axis";

        }
    }

    return inputaxis;
}

void GLFWInputController::SetMouseGameMode(bool value) 
{
    isGameMode = value;

    if (isGameMode)
    {
        if (glfwRawMouseMotionSupported ())
        {
            glfwSetInputMode (window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
        glfwSetInputMode (window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else
    {
        if (glfwRawMouseMotionSupported ())
        {
            glfwSetInputMode (window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
        }
        glfwSetInputMode (window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);        
    }
}

void GLFWInputController::Initialize () 
{
    window = glfwGetCurrentContext ();
    if (window == NULL)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "GL context/window not present";
    }

    auto keyCallback = [](GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
    {
        static_cast<GLFWInputController&>(EngineContext::GetInputController ()).KeyCallback(_key, _scancode, _action, _mods);
    };

    auto mouseButtonCallback = [](GLFWwindow* _window, int _button, int _action, int _mods)
    {
        static_cast<GLFWInputController&>(EngineContext::GetInputController ()).KeyCallback(_button, 0, _action, _mods);
    };

    auto mouseCursorCallback = [](GLFWwindow* _window, double _xPos, double _yPos)
    {
        static_cast<GLFWInputController&>(EngineContext::GetInputController ()).MouseCursorCallback(_xPos, _yPos);
    };

    auto mouseScrollCallback = [](GLFWwindow* _window, double _xOffset, double _yOffset)
    {
        static_cast<GLFWInputController&>(EngineContext::GetInputController ()).MouseScrollCallback(_xOffset, _yOffset);
    };

    glfwSetKeyCallback (window, keyCallback);
    glfwSetMouseButtonCallback (window, mouseButtonCallback);
    glfwSetCursorPosCallback (window, mouseCursorCallback);
    glfwSetScrollCallback (window, mouseScrollCallback);

    LogMessage (MSG_LOCATION) << "GLFWInputController started";
}

void GLFWInputController::Deinitialize () 
{
    glfwSetKeyCallback (window, NULL);
}

int GLFWInputController::GetGLFWKey (InputKey key)
{
    auto find = glfwKeyMap.find (key);
    if (find == glfwKeyMap.end())
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unsupported event key";
        return 0;
    }
    else 
    {
        return find->second;
    }
};

int GLFWInputController::GetGLFWTrigger (InputTrigger trigger)
{
    return trigger == InputTrigger::Press ? GLFW_PRESS : GLFW_RELEASE;
};

int GLFWInputController::GetGLFWModifiers (std::set<InputModifier> modifiers)
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

    return mods;
}

void GLFWInputController::KeyCallback (int key, int scancode, int action, int mods)
{
    // find and process mapped events

    auto findEvent = glfwKeyEventMap.find (std::make_tuple(key, action, mods));

    if (findEvent != glfwKeyEventMap.end())
    {
        findEvent->second->Set (true);
    }

    // find and process mapped axes

    auto findAxis = glfwKeyAxisMap.find (std::make_pair(key, mods));

    if (findAxis != glfwKeyAxisMap.end())
    {
        if (action == GLFW_PRESS) 
        {
            findAxis->second->Set (1.0f);
        }
        else if (action == GLFW_RELEASE)
        {
            findAxis->second->Set (0.0f);
        }
    }
}

void GLFWInputController::MouseCursorCallback(double xPos, double yPos)
{
    if (isGameMode)
    {
        if (axisMouseX)
        {
            axisMouseX->Set (static_cast<float>(prevAxisMouseX - xPos), true);
        }

        if (axisMouseY)
        {
            axisMouseY->Set (static_cast<float>(prevAxisMouseY - yPos), true);
        }
    }

    prevAxisMouseX = xPos;
    prevAxisMouseY = yPos;
    
}

void GLFWInputController::MouseScrollCallback(double xOffset, double yOffset)
{
    if (isGameMode)
    {
        if (axisMouseScrollX)
        {
            axisMouseScrollX->Set (static_cast<float>(xOffset));
        }

        if (axisMouseScrollY)
        {
            axisMouseScrollY->Set (static_cast<float>(yOffset));
        }
    }
}