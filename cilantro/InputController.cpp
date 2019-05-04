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
    for (auto& event : events)
    {
        event.second.OnFrame ();
    }

    // process axes
    // TBD
}

InputEvent* InputController::CreateEvent (std::string name, InputEventKey key, InputEventTrigger trigger) 
{
    auto search = events.find (name);

    if (search != events.end ())
    {
        LogMessage(__func__, EXIT_FAILURE) << "Event" << name << "already exists";
    }
    else 
    {
        auto inserted = events.emplace (std::piecewise_construct, std::forward_as_tuple (name), std::forward_as_tuple (key, trigger));
        if (inserted.second == true)
        {
            LogMessage(__func__) << "Created controller event" << name;
            return &(inserted.first->second);
        }
        else 
        {
            LogMessage(__func__, EXIT_FAILURE) << "Unable to create event" << name;
        }
    }

    return NULL;
}

InputAxis* InputController::CreateAxis (std::string name, InputAxis axis, float scale) 
{
    return NULL;
}

void InputController::BindEvent (std::string name, std::function<void ()> function)
{
    auto search = events.find (name);

    if (search == events.end ())
    {
        LogMessage(__func__, EXIT_FAILURE) << "Event" << name << "not found";
    }
    else 
    {
        search->second.RegisterCallback ("event", function);
    }
}


void InputController::BindAxis (std::string name, std::function<void (float)> function)
{

}

