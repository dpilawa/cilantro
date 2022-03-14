#pragma once

#include "cilantroengine.h"
#include "resource/Resource.h"
#include <string>

class CLoadableResource : public CResource
{
public:
    __EAPI CLoadableResource ();
    __EAPI CLoadableResource (const std::string& path);
    __EAPI virtual ~CLoadableResource () {};

protected:
    std::string path;
};
