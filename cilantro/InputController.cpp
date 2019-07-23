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
    float axisCompound;

    // process events
    for (auto&& event : events)
    {
        if (event->Read ()) 
        {
            InvokeCallbacks (event->GetName (), event->GetScale ());
            event->Set (false);
        }
    }

    // process axes
    for (auto&& axisvector : axes)
    {
        axisCompound = 0.0f;
        for (auto &&axis : axisvector.second)
        {
            axisCompound += axis->Read () * axis->GetScale ();
        }
        InvokeCallbacks (axisvector.first, axisCompound);
    }
}

Input<bool>* InputController::CreateInputEvent (std::string name) 
{
    events.push_back (new Input<bool> (name));

    return events.back ();
}

Input<float>* InputController::CreateInputAxis (std::string name, float scale) 
{
    axes[name].push_back (new Input<float> (name, scale));

    return axes[name].back ();
}

void InputController::BindInputEvent (std::string name, std::function<void ()> function)
{
    auto functionCallback = [function](float scale)
    {
        function ();
    };

    RegisterCallback (name, functionCallback);
}


void InputController::BindInputAxis (std::string name, std::function<void (float)> function)
{
    RegisterCallback (name, function);
}

bool InputController::IsGameMode ()
{
    return isGameMode;
}
