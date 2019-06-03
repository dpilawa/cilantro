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
            InvokeCallbacks (event->GetName (), event->GetScale ());
        }
    }

    // process axes
    // TBD
}

InputEvent* InputController::CreateInputEvent (std::string name, InputKey key, InputTrigger trigger, std::set<InputModifier> modifiers) 
{
    /* emplace event into vector */
    events.push_back (new InputEvent (name, key, trigger, modifiers));

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

