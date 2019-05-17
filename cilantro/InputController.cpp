#include "InputController.h"
#include "CallbackProvider.h"
#include "LogMessage.h"
#include <string>

InputController::InputController ()
{
}

InputController::~InputController ()
{
}

void InputController::OnFrame ()
{
    // process events
    for (auto&& event : events)
    {
        if (event->Read ()) 
        {
            InvokeCallbacks (event->GetName (), event->GetMultiplier ());
        }
    }

    // process axes
    // TBD
}

InputEvent* InputController::CreateInputEvent (std::string name, InputEventKey key, InputEventTrigger trigger, std::set<InputEventModifier> modifiers, float multiplier) 
{
    /* emplace event into vector */
    events.push_back (new InputEvent (name, key, trigger, modifiers, multiplier));

    return events.back ();
}

InputAxis* InputController::CreateInputAxis (std::string name, InputAxis axis, float scale) 
{
    return NULL;
}

void InputController::BindInputEvent (std::string name, std::function<void (float)> function)
{
    RegisterCallback (name, function);
}


void InputController::BindInputAxis (std::string name, std::function<void (float)> function)
{

}

