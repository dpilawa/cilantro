#include "LogMessage.h"

LogMessage::LogMessage ()
{
	didPrint = false;
}


LogMessage::~LogMessage ()
{
	if (didPrint) {
		std::cout << std::endl;
	}
}
