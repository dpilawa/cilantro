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
    ResourceManager();
    virtual ~ResourceManager();

    template <typename T>
    handle_t Load(const std::string& name, const std::string& path = std::string ());

    template <typename T>
    handle_t Create(const std::string& name);

    template <typename T>
    T& GetByHandle(unsigned int handle);

    template <typename T>
    T& GetByName(const std::string& name);

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

private:
    unsigned int handle;
    resources_t resources;
    std::unordered_map<std::string, unsigned int> resourceNames;
};

template <typename Base>
ResourceManager<Base>::ResourceManager ()
{
    static_assert (std::is_base_of<Resource, Base>::value, "Resaource base object must inherit from Resource");
    this->handle = 0;
    LogMessage(MSG_LOCATION) << "ResourceManager started";
}

template <typename Base>
ResourceManager<Base>::~ResourceManager ()
{
    LogMessage(MSG_LOCATION) << "ResourceManager stopped";
}

template <typename Base>
template <typename T>
handle_t ResourceManager<Base>::Load (const std::string& name, const std::string& path)
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
                newResource = std::make_shared<T> ();
            }
        }
        else
        {
            newResource = std::make_shared<T> ();
        }

        newResource->name = name;
        newResource->handle = handle;
        resources.push_back (newResource);
        resourceNames[name] = handle;
    }

    return handle++;
}

template <typename Base>
template <typename T>
handle_t ResourceManager<Base>::Create (const std::string& name)
{
    return Load<T> (name);
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

template <typename Base>
typename ResourceManager<Base>::iterator ResourceManager<Base>::begin ()
{ 
    return resources.begin(); 
}

template <typename Base>
typename ResourceManager<Base>::iterator ResourceManager<Base>::end () 
{ 
    return resources.end(); 
}

template <typename Base>
typename ResourceManager<Base>::const_iterator ResourceManager<Base>::begin () const 
{ 
    return resources.begin(); 
}

template <typename Base>
typename ResourceManager<Base>::const_iterator ResourceManager<Base>::end () const 
{ 
    return resources.end(); 
}

template <typename Base>
typename ResourceManager<Base>::const_iterator ResourceManager<Base>::cbegin () const 
{ 
    return resources.cbegin(); 
}

template <typename Base>
typename ResourceManager<Base>::const_iterator ResourceManager<Base>::cend () const 
{ 
    return resources.cend(); 
}

// deduction guide for clang (c++17)
ResourceManager() -> ResourceManager<Resource>;

#endif