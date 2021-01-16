#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "cilantroengine.h"
#include <string>

class Resource
{
public:
    __EAPI Resource (const std::string& name);
    __EAPI Resource (const std::string& name, const std::string& path);
    __EAPI virtual ~Resource ();

    __EAPI unsigned int GetHandle () const;
    __EAPI std::string GetName () const;

protected:
    unsigned int handle;
    std::string name;
    std::string path;
};

#endif