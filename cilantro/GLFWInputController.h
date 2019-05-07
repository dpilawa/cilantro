#ifndef _GLFWINPUTCONTROLLER_H_
#define _GLFWINPUTCONTROLLER_H_

#include "cilantroengine.h"
#include "InputController.h"
#include "GLFW/glfw3.h"
#include <cstddef>
#include <unordered_map>
#include <tuple>
#include <string>

namespace std 
{
    template<>
    struct hash<std::tuple<int, int, int>>
    {
        std::size_t operator() (const std::tuple<int, int, int>& k) const
        {
            return std::get<0>(k) ^ std::get<1>(k) ^ std::get<2>(k);
        }
    };
}

class GLFWInputController : public InputController
{
public:

    __EAPI GLFWInputController (GLFWwindow** window);
    __EAPI ~GLFWInputController ();

    __EAPI void Initialize ();
    __EAPI void Deinitialize ();

    __EAPI void OnFrame ();

    __EAPI InputEvent* CreateInputEvent (std::string name, InputEventKey key, InputEventTrigger trigger);
    __EAPI InputAxis*  CreateInputAxis (std::string name, InputAxis axis, float scale);

private: 
    
    void KeyCallback(int key, int scancode, int action, int mods);

    GLFWwindow** window;
    std::unordered_map<std::tuple<int, int, int>, InputEvent*> glfwEventMap;
    static std::unordered_map<InputEventKey, int> glfwKeyMap;

};


#endif // _GLFWINPUTCONTROLLER_H_