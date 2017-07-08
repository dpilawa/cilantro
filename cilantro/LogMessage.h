#ifndef _LOGMESSAGE_H_
#define _LOGMESSAGE_H_

#include <iostream>
#include <string>

class LogMessage
{
public:
	LogMessage ();
	LogMessage (std::string label);
	LogMessage (std::string label, int code);
	~LogMessage ();

	template <class P> 
	LogMessage& operator<<(const P& message);

private:
	bool didPrint;
	bool willExit;
	int errorCode;
};

template<class P>
inline LogMessage& LogMessage::operator<<(const P & message)
{
	std::cout << " " << message;
	didPrint = true;
	return *this;
}

#endif