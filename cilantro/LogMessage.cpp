#include "LogMessage.h"

LogMessage::LogMessage ()
{
	didPrint = false;
}

LogMessage::LogMessage (std::string label)
{
	std::cout << label << ": ";
	didPrint = true;
}

LogMessage::~LogMessage ()
{
	if (didPrint) {
		std::cout << std::endl;
	}
}
