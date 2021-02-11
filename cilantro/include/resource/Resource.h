#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include <string>


class Resource
{
    template <typename Base>
    friend class ResourceManager;

public:
    __EAPI Resource ();
    __EAPI virtual ~Resource ();

    __EAPI handle_t GetHandle () const;
    __EAPI std::string GetName () const;

protected:
    handle_t handle;
    std::string name;
 
};

#endif