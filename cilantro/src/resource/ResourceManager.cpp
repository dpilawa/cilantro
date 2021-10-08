#include "resource/ResourceManager.h"
#include "resource/Resource.h"
#include "scene/Material.h"
#include "scene/GameObject.h"
#include "graphics/ShaderProgram.h"
#include "graphics/RenderStage.h"

template <typename Base>
ResourceManager<Base>::ResourceManager ()
{
    static_assert (std::is_base_of<Resource, Base>::value, "Resource base object must inherit from Resource");
    this->handle = 0;
    LogMessage (MSG_LOCATION) << "ResourceManager started";
}

template <typename Base>
ResourceManager<Base>::~ResourceManager ()
{
    LogMessage (MSG_LOCATION) << "ResourceManager stopped";
}

template <typename Base>
typename ResourceManager<Base>::iterator ResourceManager<Base>::begin ()
{ 
    return resources.begin (); 
}

template <typename Base>
typename ResourceManager<Base>::iterator ResourceManager<Base>::end () 
{ 
    return resources.end (); 
}

template <typename Base>
typename ResourceManager<Base>::const_iterator ResourceManager<Base>::begin () const 
{ 
    return resources.begin (); 
}

template <typename Base>
typename ResourceManager<Base>::const_iterator ResourceManager<Base>::end () const 
{ 
    return resources.end (); 
}

template <typename Base>
typename ResourceManager<Base>::const_iterator ResourceManager<Base>::cbegin () const 
{ 
    return resources.cbegin (); 
}

template <typename Base>
typename ResourceManager<Base>::const_iterator ResourceManager<Base>::cend () const 
{ 
    return resources.cend (); 
}

template <typename Base>
std::shared_ptr<Base> ResourceManager<Base>::Push (const std::string& name, std::shared_ptr<Base> resource)
{
    auto resourceFound = resourceNames.find (name);
    if (resourceFound != resourceNames.end ()) 
    {
        LogMessage(MSG_LOCATION, EXIT_FAILURE) << "Resource" << name << "already exists";
    }
    else
    {
        resource->name = name;
        resource->handle = handle++;
        resources.push_back (resource);
        resourceNames[name] = resource->handle;
    }

    return resource;
}

// template instantiations

template __EAPI ResourceManager<Resource>::ResourceManager ();
template __EAPI ResourceManager<Resource>::~ResourceManager ();
template __EAPI ResourceManager<Resource>::iterator ResourceManager<Resource>::begin ();
template __EAPI ResourceManager<Resource>::iterator ResourceManager<Resource>::end ();
template __EAPI ResourceManager<Resource>::const_iterator ResourceManager<Resource>::begin () const;
template __EAPI ResourceManager<Resource>::const_iterator ResourceManager<Resource>::end () const;
template __EAPI ResourceManager<Resource>::const_iterator ResourceManager<Resource>::cbegin () const;
template __EAPI ResourceManager<Resource>::const_iterator ResourceManager<Resource>::cend () const;
template __EAPI std::shared_ptr<Resource> ResourceManager<Resource>::Push (const std::string& name, std::shared_ptr<Resource> resource);

template __EAPI ResourceManager<Material>::ResourceManager ();
template __EAPI ResourceManager<Material>::~ResourceManager ();
template __EAPI ResourceManager<Material>::iterator ResourceManager<Material>::begin ();
template __EAPI ResourceManager<Material>::iterator ResourceManager<Material>::end ();
template __EAPI ResourceManager<Material>::const_iterator ResourceManager<Material>::begin () const;
template __EAPI ResourceManager<Material>::const_iterator ResourceManager<Material>::end () const;
template __EAPI ResourceManager<Material>::const_iterator ResourceManager<Material>::cbegin () const;
template __EAPI ResourceManager<Material>::const_iterator ResourceManager<Material>::cend () const;
template __EAPI std::shared_ptr<Material> ResourceManager<Material>::Push (const std::string& name, std::shared_ptr<Material> resource);

template __EAPI ResourceManager<GameObject>::ResourceManager ();
template __EAPI ResourceManager<GameObject>::~ResourceManager ();
template __EAPI ResourceManager<GameObject>::iterator ResourceManager<GameObject>::begin ();
template __EAPI ResourceManager<GameObject>::iterator ResourceManager<GameObject>::end ();
template __EAPI ResourceManager<GameObject>::const_iterator ResourceManager<GameObject>::begin () const;
template __EAPI ResourceManager<GameObject>::const_iterator ResourceManager<GameObject>::end () const;
template __EAPI ResourceManager<GameObject>::const_iterator ResourceManager<GameObject>::cbegin () const;
template __EAPI ResourceManager<GameObject>::const_iterator ResourceManager<GameObject>::cend () const;
template __EAPI std::shared_ptr<GameObject> ResourceManager<GameObject>::Push (const std::string& name, std::shared_ptr<GameObject> resource);

template __EAPI ResourceManager<ShaderProgram>::ResourceManager ();
template __EAPI ResourceManager<ShaderProgram>::~ResourceManager ();
template __EAPI ResourceManager<ShaderProgram>::iterator ResourceManager<ShaderProgram>::begin ();
template __EAPI ResourceManager<ShaderProgram>::iterator ResourceManager<ShaderProgram>::end ();
template __EAPI ResourceManager<ShaderProgram>::const_iterator ResourceManager<ShaderProgram>::begin () const;
template __EAPI ResourceManager<ShaderProgram>::const_iterator ResourceManager<ShaderProgram>::end () const;
template __EAPI ResourceManager<ShaderProgram>::const_iterator ResourceManager<ShaderProgram>::cbegin () const;
template __EAPI ResourceManager<ShaderProgram>::const_iterator ResourceManager<ShaderProgram>::cend () const;
template __EAPI std::shared_ptr<ShaderProgram> ResourceManager<ShaderProgram>::Push (const std::string& name, std::shared_ptr<ShaderProgram> resource);

template __EAPI ResourceManager<RenderStage>::ResourceManager ();
template __EAPI ResourceManager<RenderStage>::~ResourceManager ();
template __EAPI ResourceManager<RenderStage>::iterator ResourceManager<RenderStage>::begin ();
template __EAPI ResourceManager<RenderStage>::iterator ResourceManager<RenderStage>::end ();
template __EAPI ResourceManager<RenderStage>::const_iterator ResourceManager<RenderStage>::begin () const;
template __EAPI ResourceManager<RenderStage>::const_iterator ResourceManager<RenderStage>::end () const;
template __EAPI ResourceManager<RenderStage>::const_iterator ResourceManager<RenderStage>::cbegin () const;
template __EAPI ResourceManager<RenderStage>::const_iterator ResourceManager<RenderStage>::cend () const;
template __EAPI std::shared_ptr<RenderStage> ResourceManager<RenderStage>::Push (const std::string& name, std::shared_ptr<RenderStage> resource);