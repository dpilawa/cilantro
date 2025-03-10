#pragma once

#include "cilantroengine.h"
#include "resource/Resource.h"
#include <string>

namespace cilantro {

class CLoadableResource : public CResource
{
public:
    __EAPI CLoadableResource ();
    __EAPI CLoadableResource (const std::string& path);
    virtual ~CLoadableResource () {};

    virtual void Load (const std::string& path) = 0;

protected:
    std::string path;
};

} // namespace cilantro
