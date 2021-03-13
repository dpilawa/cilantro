#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

#include "cilantroengine.h"
#include "system/LogMessage.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <type_traits>

class Resource;
class LoadableResource;

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

    __EAPI std::shared_ptr<Base> Push (const std::string& name, std::shared_ptr<Base> resource);

    handle_t handle;
    resources_t resources;
    std::unordered_map<std::string, handle_t> resourceNames;
};

template <typename Base>
template <typename T, typename ...Params>
T& ResourceManager<Base>::Load (const std::string& name, const std::string& path, Params&&... params)
{
    static_assert (std::is_base_of<Base, T>::value, "Invalid base class for resource");
    static_assert (std::is_base_of<LoadableResource, T>::value, "Resource is not derived from LoadableResource");
    std::shared_ptr<T> newResource;

    newResource = std::make_shared<T> (path, std::forward<Params>(params)...);
    this->Push (name, newResource);

    LogMessage (MSG_LOCATION) << "Loaded" << typeid (T).name () << name;

    return *newResource;
}

template <typename Base>
template <typename T, typename ...Params>
T& ResourceManager<Base>::Create (const std::string& name, Params&&... params)
{
    static_assert (std::is_base_of<Base, T>::value, "Invalid base class for resource");
    std::shared_ptr<T> newResource;

    newResource = std::make_shared<T> (std::forward<Params>(params)...);
    this->Push (name, newResource);

    LogMessage (MSG_LOCATION) << "Created" << typeid (T).name () << name;

    return *newResource;
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