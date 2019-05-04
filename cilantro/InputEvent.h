#ifndef _INPUTEVENT_H_
#define _INPUTEVENT_H_

#include "CallbackProvider.h"
#include <string>

enum class InputEventKey : unsigned {
    KeyA,
    KeyD,
    KeyS,
    KeyW,
    KeyEsc,
    MouseLeft,
    MouseRight
};

enum class InputEventTrigger {
    Press,
    Release
};

class InputEvent : public CallbackProvider <std::string>
{
    public:
        InputEvent (InputEventKey eventKey, InputEventTrigger eventTrigger);
        ~InputEvent ();
     
        void Set ();
        void Clear ();

        void OnFrame ();

    protected:

    private:
        InputEventKey eventKey;
        InputEventTrigger eventTrigger;
        bool eventTriggered;
};

#endif // _INPUTEVENT_H_
