#ifndef _LOGMESSAGE_H_
#define _LOGMESSAGE_H_

#include <iostream>

class LogMessage
{
public:
	LogMessage ();
	~LogMessage ();

	template <class T> 
	LogMessage& operator<<(const T& message);\
private:
	bool didPrint;
};

template<class T>
inline LogMessage& LogMessage::operator<<(const T & message)
{
	std::cout << message;
	didPrint = true;
	return *this;
}

#endif