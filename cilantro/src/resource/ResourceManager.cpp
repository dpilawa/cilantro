#include "resource/ResourceManager.h"
#include "resource/Texture.h"

template <typename Base>
ResourceManager<Base>::ResourceManager ()
{
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
unsigned int ResourceManager<Base>::Load (const std::string& name, const std::string& path)
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
        if (!path.empty ()) 
        {
            newResource = std::make_shared<T> (name, path);
        }
        else 
        {
            newResource = std::make_shared<T> (name);
        }

        resources.push_back (newResource);
        resourceNames[name] = handle;
    }

    return handle++;
}

template <typename Base>
template <typename T>
unsigned int ResourceManager<Base>::Create (const std::string& name)
{
    return Load<T> (name);
}

template <typename Base>
template <typename T>
std::shared_ptr<T> ResourceManager<Base>::GetByHandle(unsigned int handle)
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
    
    return resourcePtr;
}

template <typename Base>
template <typename T>
std::shared_ptr<T> ResourceManager<Base>::GetByName(const std::string& name)
{
    auto resourceName = resourceNames.find (name);

    if (resourceName == resourceNames.end ()) 
    {
        LogMessage(MSG_LOCATION, EXIT_FAILURE) << "Resource" << name << "not found";
    }

    return GetByHandle<T> (resourceName->second);
}

// template instantiations
template __EAPI ResourceManager<Resource>::ResourceManager ();
template __EAPI ResourceManager<Resource>::~ResourceManager ();
template __EAPI unsigned int ResourceManager<Resource>::Load<Texture> (const std::string& name, const std::string& path);
template __EAPI unsigned int ResourceManager<Resource>::Create<Texture> (const std::string& name);
template __EAPI std::shared_ptr<Texture> ResourceManager<Resource>::GetByHandle<Texture> (unsigned int handle);
template __EAPI std::shared_ptr<Texture> ResourceManager<Resource>::GetByName<Texture> (const std::string& name);
