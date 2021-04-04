#ifndef _INPUTCONTROLLER_H_
#define _INPUTCONTROLLER_H_

#include "cilantroengine.h"
#include "system/CallbackProvider.h"
#include "input/Input.h"
#include <string>
#include <functional>
#include <unordered_map>
#include <set>
#include <vector>


class InputController : public CallbackProvider <std::string, float>
{
public:
    InputController ();
    virtual ~InputController ();

    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    virtual void OnFrame ();

    virtual Input<bool>* CreateInputEvent (const std::string& name);
    virtual Input<bool>* CreateInputEvent (const std::string& name, InputKey key, InputTrigger trigger, std::set<InputModifier> modifiers) = 0;

    virtual Input<float>*  CreateInputAxis (const std::string& name, float scale);
    virtual Input<float>*  CreateInputAxis (const std::string& name, InputKey key, std::set<InputModifier> modifiers, float scale) = 0;
    virtual Input<float>*  CreateInputAxis (const std::string& name, InputAxis value, float scale) = 0;

    __EAPI void BindInputEvent (const std::string& name, std::function<void ()>);
    __EAPI void BindInputAxis (const std::string& name, std::function<void (float)>);

    virtual void SetMouseGameMode (bool value) = 0;
    __EAPI bool IsGameMode ();

private: 

    std::vector<Input<bool>*> events;
    std::unordered_map<std::string, std::vector<Input<float>*>> axes;

protected:

    bool isGameMode;

};

#endif // _INPUTCONTROLLER_H_
