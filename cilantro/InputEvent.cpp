#include "InputEvent.h"

InputEvent::InputEvent (InputEventKey key, InputEventTrigger trigger, std::set<InputEventModifier> modifiers) : eventKey (key), eventTrigger (trigger), eventModifiers (modifiers)
{
    eventTriggered = false;
}

InputEvent::~InputEvent ()
{
    
}

void InputEvent::Set ()
{
    eventTriggered = true;
}

void InputEvent::Clear ()
{
    eventTriggered = false;
}

void InputEvent::OnFrame () 
{
    if (eventTriggered)
    {
        InvokeCallbacks ("event");
        Clear ();
    }
}
