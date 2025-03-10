#pragma once

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include <string>

namespace cilantro {

class Resource
{
    template <typename Base>
    friend class ResourceManager;

public:
    Resource () { m_handle = -1; };
    virtual ~Resource () {};

    __EAPI handle_t GetHandle () const;
    __EAPI std::string GetName () const;

protected:
    handle_t m_handle;
    std::string m_name;
 
};

} // namespace cilantro
