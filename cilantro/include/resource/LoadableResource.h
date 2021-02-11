#ifndef _LOADABLERESOURCE_H_
#define _LOADABLERESOURCE_H_

#include "cilantroengine.h"
#include "resource/Resource.h"
#include <string>

class LoadableResource : public Resource
{
public:
    __EAPI LoadableResource ();
    __EAPI LoadableResource (const std::string& path);
    __EAPI virtual ~LoadableResource ();

protected:
    std::string path;
};

#endif