#include "InputEvent.h"
#include "LogMessage.h"

InputEvent::InputEvent (std::string name, InputEventKey key, InputEventTrigger trigger, std::set<InputEventModifier> modifiers, float multiplier) : 
eventName (name), eventKey (key), eventTrigger (trigger), eventModifiers (modifiers), eventMultiplier (multiplier)
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

std::string InputEvent::GetName () {
    return eventName;
}

float InputEvent::GetMultiplier () {
    return eventMultiplier;
}