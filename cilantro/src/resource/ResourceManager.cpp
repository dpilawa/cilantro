#include "resource/ResourceManager.h"
#include "resource/Texture.h"

ResourceManager::ResourceManager ()
{
    this->nextHandle = 0;
    LogMessage(__func__) << "ResourceManager started";
}

ResourceManager::~ResourceManager ()
{
    for (auto&& r : resourcesName)
    {
        LogMessage(__func__) << r.first << r.second.use_count ();
    }

    LogMessage(__func__) << "ResourceManager stopped";
}

template <typename T>
unsigned int ResourceManager::Load (const std::string& name, const std::string& path)
{
    static_assert (std::is_base_of<Resource, T>::value, "Invalid base object for resource");
    std::shared_ptr<T> newResource;

    auto resource = resourcesName.find (name);
    if (resource != resourcesName.end ()) 
    {
        LogMessage(__func__, EXIT_FAILURE) << "Resource" << name << "already exists";
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
        resourcesName[name] = newResource;
        resourcesHandle[nextHandle] = newResource;
    }

    return nextHandle++;
}

template <typename T>
unsigned int ResourceManager::Create (const std::string& name)
{
    return Load<T> (name);
}

template <typename T>
std::shared_ptr<T> ResourceManager::GetByHandle(unsigned int handle)
{

}

template <typename T>
std::shared_ptr<T> ResourceManager::GetByName(const std::string& name)
{
    auto resource = resourcesName.find (name);

    if (resource == resourcesName.end ()) 
    {
        LogMessage(__func__, EXIT_FAILURE) << "Resource" << name << "not found";
    }
    else
    {
        auto resourcePtr = std::dynamic_pointer_cast<T>(resource->second);
        if (resourcePtr)
        {
            return resource->second;
        }
        else
        {
            LogMessage(__func__, EXIT_FAILURE) << "Resource" << name << "invalid type" << typeid (T).name ();
        }
        
    }
}

// template instantiations
template __EAPI unsigned int ResourceManager::Load<Texture> (const std::string& name, const std::string& path);
template __EAPI unsigned int ResourceManager::Create<Texture> (const std::string& name);