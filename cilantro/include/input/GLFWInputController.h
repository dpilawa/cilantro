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
    std::size_t operator() (const EInputKey& k) const noexcept
    {
        return static_cast<std::size_t>(k);
    }
};

class __CEAPI GLFWInputController : public InputController
{
public:
    __EAPI GLFWInputController ();
    __EAPI virtual ~GLFWInputController ();

    __EAPI void Initialize ();
    __EAPI void Deinitialize ();

    __EAPI void OnFrame ();

    __EAPI std::shared_ptr<Input<bool>> CreateInputEvent (const std::string& name, EInputKey key, EInputTrigger trigger, std::set<EInputModifier> modifiers); 
    __EAPI std::shared_ptr<Input<float>> CreateInputAxis (const std::string& name, EInputKey key, std::set<EInputModifier> modifiers, float scale);
    __EAPI std::shared_ptr<Input<float>> CreateInputAxis (const std::string& name, EInputAxis axis, float scale);

    __EAPI void SetMouseGameMode(bool value);

private:

    int GetGLFWKey (EInputKey key);
    int GetGLFWTrigger (EInputTrigger trigger);
    int GetGLFWModifiers (std::set<EInputModifier> modifiers);

    void KeyCallback (int key, int scancode, int action, int mods);
    void MouseCursorCallback (double xPos, double yPos);      
    void MouseScrollCallback (double xOffset, double yOffset);    

    GLFWwindow* window;

    std::shared_ptr<Input<float>> axisMouseX;
    std::shared_ptr<Input<float>> axisMouseY;
    std::shared_ptr<Input<float>> axisMouseScrollX;
    std::shared_ptr<Input<float>> axisMouseScrollY;   

    double prevAxisMouseX;
    double prevAxisMouseY;

    std::unordered_map<std::tuple<int, int, int>, std::shared_ptr<Input<bool>>, TupleHash> glfwKeyEventMap;
    std::unordered_map<std::pair<int, int>, std::shared_ptr<Input<float>>, PairHash> glfwKeyAxisMap;

    static std::unordered_map<EInputKey, int, InputKeyHash> glfwKeyMap;

};

} // namespace cilantro

#endif // _GLFWINPUTCONTROLLER_H_