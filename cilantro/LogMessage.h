#ifndef _LOGMESSAGE_H_
#define _LOGMESSAGE_H_

#include <iostream>

class LogMessage
{
public:
	LogMessage ();
	~LogMessage ();

	template <class P> 
	LogMessage& operator<<(const P& message);\
private:
	bool didPrint;
};

template<class P>
inline LogMessage& LogMessage::operator<<(const P & message)
{
	std::cout << message;
	didPrint = true;
	return *this;
}

#endif