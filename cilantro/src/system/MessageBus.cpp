#include "system/MessageBus.h"
#include "system/LogMessage.h"

namespace cilantro
{

MessageBus::MessageBus ()
    : m_registry (std::make_shared<Registry> ())
{
    LogMessage () << "MessageBus started";
}

MessageBus::~MessageBus ()
{
    LogMessage () << "MessageBus stopped";
}

} // namespace cilantro