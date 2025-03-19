#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

#include "cilantroengine.h"
#include "system/LogMessage.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <type_traits>

namespace cilantro {

class Resource;
class LoadableResource;

template <typename Base = Resource>
class __CEAPI ResourceManager
{
    using TResourcesVec = std::vector<std::shared_ptr<Base>>;
    using TResourceNameMap = std::unordered_map<std::string, handle_t>;
    using iterator = typename TResourcesVec::iterator;
    using const_iterator = typename TResourcesVec::const_iterator;

public:
    __EAPI ResourceManager();
    __EAPI virtual ~ResourceManager();

    template <typename T, typename ...Params>
    T& Load (const std::string& name, const std::string& path, Params&&... params);

    template <typename T, typename ...Params>
    T& Create (const std::string& name, Params&&... params);

    template <typename T>
    T& Add (const std::string& name, std::shared_ptr<T> resource);

    template <typename T>
    T& GetByHandle (handle_t handle) const;

    template <typename T>
    T& GetByName (const std::string& name) const;

    template <typename T>
    bool HasName (const std::string& name) const;

    __EAPI iterator begin ();
    __EAPI iterator end ();
    __EAPI const_iterator begin () const;
    __EAPI const_iterator end () const;
    __EAPI const_iterator cbegin () const;
    __EAPI const_iterator cend () const;

private:

    __EAPI std::shared_ptr<Base> Push (const std::string& name, std::shared_ptr<Base> resource);

    handle_t m_nextHandle;
    TResourcesVec m_resources;
    TResourceNameMap resourceNames;
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
T& ResourceManager<Base>::Add (const std::string& name, std::shared_ptr<T> resource)
{
    static_assert (std::is_base_of<Base, T>::value, "Invalid base class for resource");
    this->Push (name, resource);

    LogMessage (MSG_LOCATION) << "Added" << typeid (T).name () << name;

    return *resource;
}

template <typename Base>
template <typename T>
T& ResourceManager<Base>::GetByHandle (handle_t handle) const
{
    if (handle >= this->m_nextHandle)
    {
        LogMessage(MSG_LOCATION, EXIT_FAILURE) << "Resource handle" << handle << "out of bounds";
    }

    auto resource = m_resources[handle];
    auto resourcePtr = std::dynamic_pointer_cast<T> (resource);
        
    if (resourcePtr == nullptr)
    {
        LogMessage(MSG_LOCATION, EXIT_FAILURE) << "Resource" << resource->GetName () << "invalid type" << typeid (T).name ();
    }
    
    return *(resourcePtr.get ());
}

template <typename Base>
template <typename T>
T& ResourceManager<Base>::GetByName (const std::string& name) const
{
    auto resourceName = resourceNames.find (name);

    if (resourceName == resourceNames.end ()) 
    {
        LogMessage(MSG_LOCATION, EXIT_FAILURE) << "Resource" << name << "not found";
    }

    return GetByHandle<T> (resourceName->second);
}

template <typename Base>
template <typename T>
bool ResourceManager<Base>::HasName (const std::string& name) const
{
    auto resourceName = resourceNames.find (name);

    if (resourceName == resourceNames.end ()) 
    {
        return false;
    }

    auto resource = m_resources[resourceName->second];
    auto resourcePtr = std::dynamic_pointer_cast<T> (resource);
        
    if (resourcePtr == nullptr)
    {
        return false;
    }

    return true;
}

// deduction guide for clang (c++17)
ResourceManager () -> ResourceManager<Resource>;

} // namespace cilantro

#endif