#ifndef _INPUTCONTROLLER_H_
#define _INPUTCONTROLLER_H_

#include "cilantroengine.h"
#include "CallbackProvider.h"
#include "Input.h"
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

    virtual Input<bool>* CreateInputEvent (std::string name);
    virtual Input<bool>* CreateInputEvent (std::string name, InputKey key, InputTrigger trigger, std::set<InputModifier> modifiers) = 0;

    virtual Input<float>*  CreateInputAxis (std::string name, float scale);
    virtual Input<float>*  CreateInputAxis (std::string name, InputKey key, std::set<InputModifier> modifiers, float scale) = 0;
    virtual Input<float>*  CreateInputAxis (std::string name, InputAxis value, float scale) = 0;

    __EAPI void BindInputEvent (std::string name, std::function<void (float)>);
    __EAPI void BindInputAxis (std::string name, std::function<void (float)>);


private: 
    
    std::vector<Input<bool>*> events;
    std::unordered_map<std::string, std::vector<Input<float>*>> axes;

};

#endif // _INPUTCONTROLLER_H_
