#ifndef _INPUTEVENT_H_
#define _INPUTEVENT_H_

#include "CallbackProvider.h"
#include <string>
#include <set>

enum class InputEventKey : unsigned {
    /* keyboard events */
    KeyA,
    KeyB,
    KeyC,
    KeyD,
    KeyE,
    KeyF,
    KeyG,
    KeyH,
    KeyI,
    KeyJ,
    KeyK,
    KeyL,
    KeyM,
    KeyN,
    KeyO,
    KeyP,
    KeyQ,
    KeyR,
    KeyS,
    KeyT,
    KeyU,
    KeyV,
    KeyW,
    KeyX,
    KeyY,
    KeyZ,
    Key1,
    Key2,
    Key3,
    Key4,
    Key5,
    Key6,
    Key7,
    Key8,
    Key9,
    Key0,    
    KeyUp,
    KeyDown,
    KeyLeft,
    KeyRight,
    KeyEsc,
    KeyEnter,
    KeySpace,
    KeyLeftControl,
    KeyRightControl,
    KeyLeftShift,
    KeyRightShift,
    KeyLeftAlt,
    KeyRightAlt,    
    /* mouse events */
    MouseLeft,
    MouseMiddle,
    MouseRight,
};

enum class InputEventTrigger : unsigned {
    Press,
    Release
};

enum class InputEventModifier : unsigned {
    Control,
    Alt,
    Shift
};

class InputEvent : public CallbackProvider <std::string>
{
    public:
        InputEvent (InputEventKey key, InputEventTrigger trigger, std::set<InputEventModifier> modifiers);
        ~InputEvent ();
     
        void Set ();
        void Clear ();

        void OnFrame ();

    protected:

    private:
        InputEventKey eventKey;
        InputEventTrigger eventTrigger;
        std::set<InputEventModifier> eventModifiers;
        bool eventTriggered;
};

#endif // _INPUTEVENT_H_
