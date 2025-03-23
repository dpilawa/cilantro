#include "input/InputController.h"
#include "system/LogMessage.h"
#include "system/Game.h"
#include "system/MessageBus.h"
#include "system/Message.h"
#include <memory>
#include <string>

namespace cilantro {

InputController::InputController (std::shared_ptr<Game> game) : m_game (game)
{
}

InputController::~InputController ()
{
}

std::shared_ptr<Game> InputController::GetGame ()
{
    return m_game.lock ();
}

void InputController::OnFrame ()
{
    float axisCompound;

    // process events
    for (auto&& event : events)
    {
        if (event->Read ()) 
        {
            GetGame ()->GetMessageBus ()->Publish<InputEventMessage> (std::make_shared<InputEventMessage> (event->GetName (), event->GetScale ()));
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
        GetGame ()->GetMessageBus ()->Publish<InputAxisMessage> (std::make_shared<InputAxisMessage> (axisvector.first, axisCompound));
    }
}

std::shared_ptr<Input<bool>> InputController::CreateInputEvent (const std::string& name) 
{
    auto event = std::make_shared<Input<bool>>(name);
    events.push_back(event);
    return event;
}

std::shared_ptr<Input<float>> InputController::CreateInputAxis (const std::string& name, float scale) 
{
    auto axis = std::make_shared<Input<float>>(name, scale);
    axes[name].push_back(axis);
    return axis;
}

void InputController::BindInputEvent (const std::string& name, std::function<void ()> function)
{
    GetGame ()->GetMessageBus ()->Subscribe<InputEventMessage> ([name, function](const std::shared_ptr<InputEventMessage>& message) 
    { 
        if (message->GetEvent () == name)
        {
            function ();
        }
    });
}

void InputController::BindInputAxis (const std::string& name, std::function<void (float)> function)
{
    GetGame ()->GetMessageBus ()->Subscribe<InputAxisMessage> ([name, function](const std::shared_ptr<InputAxisMessage>& message) 
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
