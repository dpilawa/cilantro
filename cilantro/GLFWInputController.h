#ifndef _GLFWINPUTCONTROLLER_H_
#define _GLFWINPUTCONTROLLER_H_

#include "cilantroengine.h"
#include "InputController.h"
#include "GLFW/glfw3.h"
#include <type_traits>
#include <cstddef>
#include <unordered_map>
#include <tuple>
#include <string>
#include <set>

struct TupleHash
{
    std::size_t operator() (const std::tuple<int, int, int>& k) const noexcept
    {
        return std::get<0>(k) ^ std::get<1>(k) ^ std::get<2>(k);
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

    __EAPI GLFWInputController (GLFWwindow** window);
    __EAPI ~GLFWInputController ();

    __EAPI void Initialize ();
    __EAPI void Deinitialize ();

    __EAPI void OnFrame ();

    __EAPI InputEvent* CreateInputEvent (std::string name, InputKey key, InputTrigger trigger, std::set<InputModifier> modifiers); 
    __EAPI InputAxis*  CreateInputAxis (std::string name, InputAxis axis, float scale);

private: 
    
    void KeyCallback(int key, int scancode, int action, int mods);
    void MouseCursorCallback(double xpos, double ypos);

    GLFWwindow** window;
    std::unordered_map<std::tuple<int, int, int>, InputEvent*, TupleHash> glfwEventMap;
    static std::unordered_map<InputKey, int, InputKeyHash> glfwKeyMap;

};


#endif // _GLFWINPUTCONTROLLER_H_