#pragma once

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include <string>

namespace cilantro {

class __CEAPI Resource
{
    template <typename Base>
    friend class ResourceManager;

public:
    __EAPI Resource ();
    __EAPI virtual ~Resource ();

    __EAPI handle_t GetHandle () const;
    __EAPI std::string GetName () const;
    __EAPI std::shared_ptr<Resource> GetPointer () const;

protected:
    handle_t m_handle;
    std::string m_name;
    std::weak_ptr<Resource> m_pointer;
 
};

} // namespace cilantro
