#pragma once

#include "cilantroengine.h"
#include "resource/Resource.h"
#include <string>

namespace cilantro {

class LoadableResource : public Resource
{
public:
    __EAPI LoadableResource ();
    __EAPI LoadableResource (const std::string& path);
    virtual ~LoadableResource () {};

    virtual void Load (const std::string& path) = 0;

protected:
    std::string path;
};

} // namespace cilantro
