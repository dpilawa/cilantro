#include "system/MessageBus.h"
#include "system/LogMessage.h"

MessageBus::MessageBus ()
{
    LogMessage () << "MessageBus started";
}

MessageBus::~MessageBus ()
{
    LogMessage () << "MessageBus stopped";
}