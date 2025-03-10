#include "system/MessageBus.h"
#include "system/LogMessage.h"

namespace cilantro
{

MessageBus::MessageBus ()
{
    LogMessage () << "MessageBus started";
}

MessageBus::~MessageBus ()
{
    LogMessage () << "MessageBus stopped";
}

} // namespace cilantro