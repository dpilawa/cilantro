#include "cilantroengine.h"
#include "input/GLFWInputController.h"
#include "system/Game.h"
#include "system/LogMessage.h"
#include "input/Input.h"

namespace cilantro {

std::unordered_map<EInputKey, int, InputKeyHash> GLFWInputController::glfwKeyMap {
        {EInputKey::KeyA, GLFW_KEY_A},
        {EInputKey::KeyB, GLFW_KEY_B},
        {EInputKey::KeyC, GLFW_KEY_C},
        {EInputKey::KeyD, GLFW_KEY_D},
        {EInputKey::KeyE, GLFW_KEY_E},
        {EInputKey::KeyF, GLFW_KEY_F},
        {EInputKey::KeyG, GLFW_KEY_G},
        {EInputKey::KeyH, GLFW_KEY_H},
        {EInputKey::KeyI, GLFW_KEY_I},
        {EInputKey::KeyJ, GLFW_KEY_J},
        {EInputKey::KeyK, GLFW_KEY_K},
        {EInputKey::KeyL, GLFW_KEY_L},
        {EInputKey::KeyM, GLFW_KEY_M},
        {EInputKey::KeyN, GLFW_KEY_N},
        {EInputKey::KeyO, GLFW_KEY_O},
        {EInputKey::KeyP, GLFW_KEY_P},
        {EInputKey::KeyQ, GLFW_KEY_Q},
        {EInputKey::KeyR, GLFW_KEY_R},
        {EInputKey::KeyS, GLFW_KEY_S},
        {EInputKey::KeyT, GLFW_KEY_T},
        {EInputKey::KeyU, GLFW_KEY_U},
        {EInputKey::KeyV, GLFW_KEY_V},
        {EInputKey::KeyW, GLFW_KEY_W},
        {EInputKey::KeyX, GLFW_KEY_X},
        {EInputKey::KeyY, GLFW_KEY_Y},
        {EInputKey::KeyZ, GLFW_KEY_Z},
        {EInputKey::Key1, GLFW_KEY_1},
        {EInputKey::Key2, GLFW_KEY_2},
        {EInputKey::Key3, GLFW_KEY_3},
        {EInputKey::Key4, GLFW_KEY_4},
        {EInputKey::Key5, GLFW_KEY_5},
        {EInputKey::Key6, GLFW_KEY_6},
        {EInputKey::Key7, GLFW_KEY_7},
        {EInputKey::Key8, GLFW_KEY_8},
        {EInputKey::Key9, GLFW_KEY_9},
        {EInputKey::Key0,  GLFW_KEY_0},
        {EInputKey::KeyUp, GLFW_KEY_UP},
        {EInputKey::KeyDown, GLFW_KEY_DOWN},
        {EInputKey::KeyLeft, GLFW_KEY_LEFT},
        {EInputKey::KeyRight, GLFW_KEY_RIGHT},
        {EInputKey::KeyEsc, GLFW_KEY_ESCAPE},
        {EInputKey::KeyEnter, GLFW_KEY_ENTER},
        {EInputKey::KeySpace, GLFW_KEY_SPACE},
        {EInputKey::KeyLeftControl, GLFW_KEY_LEFT_CONTROL},
        {EInputKey::KeyRightControl, GLFW_KEY_RIGHT_CONTROL},
        {EInputKey::KeyLeftShift, GLFW_KEY_LEFT_SHIFT},
        {EInputKey::KeyRightShift, GLFW_KEY_RIGHT_SHIFT},
        {EInputKey::KeyLeftAlt, GLFW_KEY_LEFT_ALT},
        {EInputKey::KeyRightAlt,  GLFW_KEY_RIGHT_ALT},
        {EInputKey::MouseLeft, GLFW_MOUSE_BUTTON_LEFT},
        {EInputKey::MouseMiddle, GLFW_MOUSE_BUTTON_MIDDLE},
        {EInputKey::MouseRight, GLFW_MOUSE_BUTTON_RIGHT}
};

GLFWInputController::GLFWInputController () : InputController ()
{
    axisMouseX = nullptr;
    axisMouseY = nullptr;
    axisMouseScrollX = nullptr;
    axisMouseScrollY = nullptr;

    isGameMode = false;

    Initialize ();
}

GLFWInputController::~GLFWInputController ()
{

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
        static_cast<GLFWInputController&>(Game::GetInputController ()).KeyCallback(_key, _scancode, _action, _mods);
    };

    auto mouseButtonCallback = [](GLFWwindow* _window, int _button, int _action, int _mods)
    {
        static_cast<GLFWInputController&>(Game::GetInputController ()).KeyCallback(_button, 0, _action, _mods);
    };

    auto mouseCursorCallback = [](GLFWwindow* _window, double _xPos, double _yPos)
    {
        static_cast<GLFWInputController&>(Game::GetInputController ()).MouseCursorCallback(_xPos, _yPos);
    };

    auto mouseScrollCallback = [](GLFWwindow* _window, double _xOffset, double _yOffset)
    {
        static_cast<GLFWInputController&>(Game::GetInputController ()).MouseScrollCallback(_xOffset, _yOffset);
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

void GLFWInputController::OnFrame ()
{
    glfwPollEvents ();
    InputController::OnFrame ();
}

Input<bool>* GLFWInputController::CreateInputEvent (const std::string& name, EInputKey key, EInputTrigger trigger, std::set<EInputModifier> modifiers)
{
    Input<bool>* inputevent = InputController::CreateInputEvent (name);

    if (inputevent) 
    {
        glfwKeyEventMap[std::make_tuple (GetGLFWKey (key), GetGLFWTrigger (trigger), GetGLFWModifiers (modifiers))] = inputevent;
        LogMessage (MSG_LOCATION) << "Mapped key event" << GetGLFWKey (key) << GetGLFWTrigger (trigger) << GetGLFWModifiers (modifiers);
    }

    return inputevent;
}


Input<float>* GLFWInputController::CreateInputAxis (const std::string& name, EInputKey key, std::set<EInputModifier> modifiers, float scale) 
{
    Input<float>* inputaxis = InputController::CreateInputAxis (name, scale);

    if (inputaxis) 
    {
        glfwKeyAxisMap[std::make_pair (GetGLFWKey (key), GetGLFWModifiers (modifiers))] = inputaxis;
        LogMessage (MSG_LOCATION) << "Mapped key axis" << GetGLFWKey (key) << GetGLFWModifiers (modifiers);
    }

    return inputaxis;
}

Input<float>* GLFWInputController::CreateInputAxis (const std::string& name, EInputAxis axis, float scale) 
{
    Input<float>* inputaxis = InputController::CreateInputAxis (name, scale);

    if (inputaxis)
    {
        switch (axis)
        {
            case EInputAxis::MouseX :
                axisMouseX = inputaxis;
                break;

            case EInputAxis::MouseY :
                axisMouseY = inputaxis;
                break;

            case EInputAxis::MouseScrollX :
                axisMouseScrollX = inputaxis;
                break;

            case EInputAxis::MouseScrollY :
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

int GLFWInputController::GetGLFWKey (EInputKey key)
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

int GLFWInputController::GetGLFWTrigger (EInputTrigger trigger)
{
    return trigger == EInputTrigger::Press ? GLFW_PRESS : GLFW_RELEASE;
};

int GLFWInputController::GetGLFWModifiers (std::set<EInputModifier> modifiers)
{
    int mods = 0;

    for (auto& m : modifiers) 
    {
        switch (m) 
        {
            case EInputModifier::Alt : mods |= GLFW_MOD_ALT;
                break;
            case EInputModifier::Control : mods |= GLFW_MOD_CONTROL;
                break;
            case EInputModifier::Shift : mods |= GLFW_MOD_SHIFT;
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

} // namespace cilantro