#ifndef _INPUTCONTROLLER_H_
#define _INPUTCONTROLLER_H_

#include "cilantroengine.h"
#include "CallbackProvider.h"
#include "InputEvent.h"
#include "InputAxis.h"
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

    virtual InputEvent* CreateInputEvent (std::string name, InputEventKey key, InputEventTrigger trigger, std::set<InputEventModifier> modifiers, float multiplier);
    virtual InputAxis*  CreateInputAxis (std::string name, InputAxis axis, float scale);

    __EAPI void BindInputEvent (std::string name, std::function<void (float)>);
    __EAPI void BindInputAxis (std::string name, std::function<void (float)>);


private: 
    
    std::vector<InputEvent*> events;
    std::vector<InputAxis*> axes;

};

#endif // _INPUTCONTROLLER_H_
