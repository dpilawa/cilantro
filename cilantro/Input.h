#ifndef _INPUT_H_
#define _INPUT_H_

#include <string>

enum class InputKey: unsigned 
{
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
    MouseRight
};

enum class InputValue : unsigned
{
    /* mouse axes */
    MouseX,
    MouseY
};

enum class InputTrigger : unsigned 
{
    Press,
    Release
};

enum class InputModifier : unsigned
{
    Control,
    Alt,
    Shift
};

class Input
{
    public:
        Input (std::string name, float scale);
        Input (std::string name);
        ~Input();
     
        std::string GetName ();
        float GetScale ();

    private:
        std::string inputName;
        float inputScale;
};

#endif // _INPUT_H_
