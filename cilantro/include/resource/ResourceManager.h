#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

#include "cilantroengine.h"
#include "system/LogMessage.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

class Resource;

template <typename Base = Resource>
class ResourceManager
{
    using resources_t = std::vector<std::shared_ptr<Base>>;
    using iterator = typename resources_t::iterator;
    using const_iterator = typename resources_t::const_iterator;

public:
    __EAPI ResourceManager();
    __EAPI virtual ~ResourceManager();

    template <typename T, typename ...Params>
    T& Load(const std::string& name, const std::string& path, Params&&... params);

    template <typename T, typename ...Params>
    T& Create(const std::string& name, Params&&... params);

    template <typename T>
    T& GetByHandle(unsigned int handle);

    template <typename T>
    T& GetByName(const std::string& name);

    __EAPI iterator begin();
    __EAPI iterator end();
    __EAPI const_iterator begin() const;
    __EAPI const_iterator end() const;
    __EAPI const_iterator cbegin() const;
    __EAPI const_iterator cend() const;

private:
    unsigned int handle;
    resources_t resources;
    std::unordered_map<std::string, unsigned int> resourceNames;
};

template <typename Base>
template <typename T, typename ...Params>
T& ResourceManager<Base>::Load (const std::string& name, const std::string& path, Params&&... params)
{
    static_assert (std::is_base_of<Base, T>::value, "Invalid base object for resource");
    std::shared_ptr<T> newResource;

    auto resource = resourceNames.find (name);
    if (resource != resourceNames.end ()) 
    {
        LogMessage(MSG_LOCATION, EXIT_FAILURE) << "Resource" << name << "already exists";
    }
    else
    {
        if constexpr (std::is_base_of<LoadableResource, T>::value)
        {
            if (!path.empty ()) 
            {
                newResource = std::make_shared<T> (path);
            }
            else 
            {
                newResource = std::make_shared<T> (std::forward<Params>(params)...);
            }
        }
        else
        {
            newResource = std::make_shared<T> (std::forward<Params>(params)...);
        }

        newResource->name = name;
        newResource->handle = handle++;
        resources.push_back (newResource);
        resourceNames[name] = newResource->handle;
    }

    return *newResource;
}

template <typename Base>
template <typename T, typename ...Params>
T& ResourceManager<Base>::Create (const std::string& name, Params&&... params)
{
    return Load<T> (name, std::string (), params...);
}

template <typename Base>
template <typename T>
T& ResourceManager<Base>::GetByHandle(handle_t handle)
{
    if (handle >= this->handle)
    {
        LogMessage(MSG_LOCATION, EXIT_FAILURE) << "Resource handle" << handle << "out of bounds";
    }

    auto resource = resources[handle];
    auto resourcePtr = std::dynamic_pointer_cast<T> (resource);
        
    if (resourcePtr == nullptr)
    {
        LogMessage(MSG_LOCATION, EXIT_FAILURE) << "Resource" << resource->GetName () << "invalid type" << typeid (T).name ();
    }
    
    return *(resourcePtr.get ());
}

template <typename Base>
template <typename T>
T& ResourceManager<Base>::GetByName(const std::string& name)
{
    auto resourceName = resourceNames.find (name);

    if (resourceName == resourceNames.end ()) 
    {
        LogMessage(MSG_LOCATION, EXIT_FAILURE) << "Resource" << name << "not found";
    }

    return GetByHandle<T> (resourceName->second);
}

// deduction guide for clang (c++17)
ResourceManager() -> ResourceManager<Resource>;

#endif