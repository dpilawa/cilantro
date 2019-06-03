#include "InputEvent.h"
#include "LogMessage.h"

InputEvent::InputEvent (std::string name, InputKey key, InputTrigger trigger, std::set<InputModifier> modifiers) : 
Input(name), eventKey (key), eventTrigger (trigger), eventModifiers (modifiers)
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

bool InputEvent::Read ()
{
    bool eventState = eventTriggered;
    
    Clear ();
    return eventState;
}
