#include "cilantroengine.h"
#include "system/LogMessage.h"

namespace cilantro
{

LogMessage::LogMessage ()
{
    didPrint = false;
    willExit = false;
}

LogMessage::LogMessage (const std::string& label)
{
    std::cout << label << ":";
    didPrint = true;
    willExit = false;
}

LogMessage::LogMessage (const std::string& label, int code) : LogMessage(label)
{
    willExit = true;
    errorCode = code;
}

LogMessage::~LogMessage ()
{
    if (didPrint) 
    {
        std::cout << std::endl;
    }

    if (willExit)
    {
        std::exit (errorCode);
    }
}

} // namespace cilantro