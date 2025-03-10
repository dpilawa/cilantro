#ifndef _GLFWINPUTCONTROLLER_H_
#define _GLFWINPUTCONTROLLER_H_

#include "cilantroengine.h"
#include "input/InputController.h"
#include "GLFW/glfw3.h"
#include <type_traits>
#include <cstddef>
#include <unordered_map>
#include <tuple>
#include <string>
#include <set>

namespace cilantro {

struct TupleHash
{
    std::size_t operator() (const std::tuple<int, int, int>& k) const noexcept
    {
        return std::get<0>(k) ^ std::get<1>(k) ^ std::get<2>(k);
    }
};

struct PairHash
{
    std::size_t operator() (const std::pair<int, int>& k) const noexcept
    {
        return k.first ^ k.second;
    }
};

struct InputKeyHash
{
    std::size_t operator() (const InputKey& k) const noexcept
    {
        return static_cast<std::size_t>(k);
    }
};

class GLFWInputController : public InputController
{
public:
    __EAPI GLFWInputController ();
    __EAPI ~GLFWInputController ();

    __EAPI void OnFrame ();

    __EAPI Input<bool>* CreateInputEvent (const std::string& name, InputKey key, InputTrigger trigger, std::set<InputModifier> modifiers); 
    __EAPI Input<float>* CreateInputAxis (const std::string& name, InputKey key, std::set<InputModifier> modifiers, float scale);
    __EAPI Input<float>* CreateInputAxis (const std::string& name, InputAxis axis, float scale);

    __EAPI void SetMouseGameMode(bool value);

private:

    void Initialize ();
    void Deinitialize ();

    int GetGLFWKey (InputKey key);
    int GetGLFWTrigger (InputTrigger trigger);
    int GetGLFWModifiers (std::set<InputModifier> modifiers);

    void KeyCallback (int key, int scancode, int action, int mods);
    void MouseCursorCallback (double xPos, double yPos);      
    void MouseScrollCallback (double xOffset, double yOffset);    

    GLFWwindow* window;

    Input<float>* axisMouseX;
    Input<float>* axisMouseY;
    Input<float>* axisMouseScrollX;
    Input<float>* axisMouseScrollY;   

    double prevAxisMouseX;
    double prevAxisMouseY;

    std::unordered_map<std::tuple<int, int, int>, Input<bool>*, TupleHash> glfwKeyEventMap;
    std::unordered_map<std::pair<int, int>, Input<float>*, PairHash> glfwKeyAxisMap;

    static std::unordered_map<InputKey, int, InputKeyHash> glfwKeyMap;

};

} // namespace cilantro

#endif // _GLFWINPUTCONTROLLER_H_