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

enum class InputAxis : unsigned
{
    /* mouse axes */
    MouseX,
    MouseY,
    /* mouse scroll */
    MouseScrollX,
    MouseScrollY
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

template<typename T>
class Input
{
    public:
        Input (std::string name, float scale);
        Input (std::string name);
        ~Input();
     
        std::string GetName ();
        float GetScale ();

        void Set (T value);
        void Set (T value, float shouldClearOnRead);
        T Read ();
        void Clear();
        
    private:
        T inputValue{};
        std::string inputName;
        float inputScale;
        bool clearOnRead;
};

template<typename T>
inline Input<T>::Input (std::string name, float scale) : 
inputName (name), inputScale(scale)
{
    clearOnRead = false;
}

template<typename T>
inline Input<T>::Input (std::string name) : 
inputName (name), inputScale(1.0f)
{

}

template<typename T>
inline Input<T>::~Input ()
{   
}


template<typename T>
inline std::string Input<T>::GetName () {
    return inputName;
}

template<typename T>
inline float Input<T>::GetScale () 
{
    return inputScale;
}

template<typename T>
inline void Input<T>::Set (T value) 
{
    Set (value, false);
}

template<typename T>
inline void Input<T>::Set (T value, float shouldClearOnRead) 
{
    inputValue = value;
    clearOnRead = shouldClearOnRead;
}

template<typename T>
inline T Input<T>::Read () 
{
    T returnValue;

    returnValue = inputValue;

    if (clearOnRead) 
    {
        Clear ();
        clearOnRead = false;
    }

    return returnValue;
}

template<>
inline void Input<bool>::Clear ()
{
    inputValue = false;
}

template<>
inline void Input<float>::Clear ()
{
    inputValue = 0.0f;
}

#endif // _INPUT_H_
