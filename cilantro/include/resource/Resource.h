#pragma once

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include <string>

class CResource
{
    template <typename Base>
    friend class CResourceManager;

public:
    CResource () {};
    virtual ~CResource () {};

    __EAPI handle_t GetHandle () const;
    __EAPI std::string GetName () const;

protected:
    handle_t m_handle;
    std::string m_name;
 
};
