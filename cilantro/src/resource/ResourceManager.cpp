#include "resource/ResourceManager.h"
#include "resource/Resource.h"
#include "scene/GameScene.h"
#include "scene/Material.h"
#include "scene/GameObject.h"
#include "scene/AnimationObject.h"
#include "scene/AnimationProperty.h"
#include "graphics/ShaderProgram.h"
#include "graphics/RenderStage.h"

#define INSTANTIATE_RESOURCE_MANAGER(ClassName) \
template __EAPI ResourceManager<ClassName>::ResourceManager (); \
template __EAPI ResourceManager<ClassName>::~ResourceManager (); \
template __EAPI size_t ResourceManager<ClassName>::GetCount () const; \
template __EAPI ResourceManager<ClassName>::iterator ResourceManager<ClassName>::begin (); \
template __EAPI ResourceManager<ClassName>::iterator ResourceManager<ClassName>::end (); \
template __EAPI ResourceManager<ClassName>::const_iterator ResourceManager<ClassName>::begin () const; \
template __EAPI ResourceManager<ClassName>::const_iterator ResourceManager<ClassName>::end () const; \
template __EAPI ResourceManager<ClassName>::const_iterator ResourceManager<ClassName>::cbegin () const; \
template __EAPI ResourceManager<ClassName>::const_iterator ResourceManager<ClassName>::cend () const; \
template __EAPI std::shared_ptr<ClassName> ResourceManager<ClassName>::Push (const std::string& name, std::shared_ptr<ClassName> resource);

namespace cilantro {

template <typename Base>
ResourceManager<Base>::ResourceManager ()
{
    static_assert (std::is_base_of<Resource, Base>::value, "Resource base object must inherit from CResource");
    this->m_nextHandle = 0;
    LogMessage (MSG_LOCATION) << "ResourceManager started";
}

template <typename Base>
ResourceManager<Base>::~ResourceManager ()
{
    LogMessage (MSG_LOCATION) << "ResourceManager stopped";
}

template <typename Base>
size_t ResourceManager<Base>::GetCount () const
{
    return m_resources.size ();
}

template <typename Base>
typename ResourceManager<Base>::iterator ResourceManager<Base>::begin ()
{ 
    return m_resources.begin (); 
}

template <typename Base>
typename ResourceManager<Base>::iterator ResourceManager<Base>::end () 
{ 
    return m_resources.end (); 
}

template <typename Base>
typename ResourceManager<Base>::const_iterator ResourceManager<Base>::begin () const 
{ 
    return m_resources.begin (); 
}

template <typename Base>
typename ResourceManager<Base>::const_iterator ResourceManager<Base>::end () const 
{ 
    return m_resources.end (); 
}

template <typename Base>
typename ResourceManager<Base>::const_iterator ResourceManager<Base>::cbegin () const 
{ 
    return m_resources.cbegin (); 
}

template <typename Base>
typename ResourceManager<Base>::const_iterator ResourceManager<Base>::cend () const 
{ 
    return m_resources.cend (); 
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
        resource->m_name = name;
        resource->m_handle = m_nextHandle++;
        m_resources.push_back (resource);
        resourceNames[name] = resource->m_handle;
    }

    return resource;
}

// template instantiations

INSTANTIATE_RESOURCE_MANAGER(Resource)
INSTANTIATE_RESOURCE_MANAGER(GameScene)
INSTANTIATE_RESOURCE_MANAGER(Material)
INSTANTIATE_RESOURCE_MANAGER(GameObject)
INSTANTIATE_RESOURCE_MANAGER(ShaderProgram)
INSTANTIATE_RESOURCE_MANAGER(RenderStage)
INSTANTIATE_RESOURCE_MANAGER(AnimationProperty<float>)
INSTANTIATE_RESOURCE_MANAGER(AnimationProperty<Vector3f>)
INSTANTIATE_RESOURCE_MANAGER(AnimationProperty<Quaternion>)

} // namespace cilantro
