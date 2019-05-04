#ifndef _INPUTCONTROLLER_H_
#define _INPUTCONTROLLER_H_

#include "cilantroengine.h"
#include "InputEvent.h"
#include "InputAxis.h"
#include <string>
#include <functional>
#include <unordered_map>


class InputController
{
public:

    InputController ();
    virtual ~InputController ();

    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    virtual void OnFrame ();

    virtual InputEvent* CreateEvent (std::string name, InputEventKey key, InputEventTrigger trigger);
    virtual InputAxis*  CreateAxis (std::string name, InputAxis axis, float scale);

    __EAPI void BindEvent (std::string name, std::function<void ()>);
    __EAPI void BindAxis (std::string name, std::function<void (float)>);


private: 
    
    std::unordered_map <std::string, InputEvent> events;
    std::unordered_map <std::string, InputAxis> axes;

};

#endif // _INPUTCONTROLLER_H_
