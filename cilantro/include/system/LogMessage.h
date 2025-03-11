#ifndef _LOGMESSAGE_H_
#define _LOGMESSAGE_H_

#include "cilantroengine.h"
#include <string>
#include <iostream>

#define MSG_LOCATION typeid (*this).name () + std::string (": ") + std::string (__func__)

namespace cilantro {

class __CEAPI LogMessage
{
public:
    __EAPI LogMessage ();
    __EAPI LogMessage (const std::string& label);
    __EAPI LogMessage (const std::string& label, int code);
    __EAPI ~LogMessage ();

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
    if (didPrint)
    {
        std::cout << " ";
    }

    std::cout << message;
    didPrint = true;

    return *this;
}

} // namespace cilantro

#endif