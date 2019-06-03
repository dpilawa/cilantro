#ifndef _INPUTEVENT_H_
#define _INPUTEVENT_H_

#include "Input.h"
#include <string>
#include <set>


class InputEvent : public Input
{
    public:
        InputEvent (std::string name, InputKey key, InputTrigger trigger, std::set<InputModifier> modifiers);
        ~InputEvent ();
     
        void Set ();
        void Clear ();        
        bool Read ();

    protected:

    private:
        InputKey eventKey;
        InputTrigger eventTrigger;
        std::set<InputModifier> eventModifiers;
        bool eventTriggered;
};

#endif // _INPUTEVENT_H_
