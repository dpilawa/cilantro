#include "input/InputController.h"
#include "system/LogMessage.h"
#include "system/Game.h"
#include "system/MessageBus.h"
#include "system/Message.h"

#include <string>

namespace cilantro {

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
            Game::GetMessageBus ().Publish<InputEventMessage> (std::make_shared<InputEventMessage> (event->GetName (), event->GetScale ()));
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
        Game::GetMessageBus ().Publish<InputAxisMessage> (std::make_shared<InputAxisMessage> (axisvector.first, axisCompound));
    }
}

Input<bool>* InputController::CreateInputEvent (const std::string& name) 
{
    events.push_back (new Input<bool> (name));

    return events.back ();
}

Input<float>* InputController::CreateInputAxis (const std::string& name, float scale) 
{
    axes[name].push_back (new Input<float> (name, scale));

    return axes[name].back ();
}

void InputController::BindInputEvent (const std::string& name, std::function<void ()> function)
{
    Game::GetMessageBus ().Subscribe<InputEventMessage> ([name, function](const std::shared_ptr<InputEventMessage>& message) 
    { 
        if (message->GetEvent () == name)
        {
            function ();
        }
    });
}

void InputController::BindInputAxis (const std::string& name, std::function<void (float)> function)
{
    Game::GetMessageBus ().Subscribe<InputAxisMessage> ([name, function](const std::shared_ptr<InputAxisMessage>& message) 
    { 
        if (message->GetEvent () == name)
        {
            function (message->GetValue ());
        }
    });
}

bool InputController::IsGameMode ()
{
    return isGameMode;
}

} // namespace cilantro
