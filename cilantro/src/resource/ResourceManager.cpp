#include "resource/ResourceManager.h"
#include "resource/Resource.h"
#include "scene/GameScene.h"
#include "scene/Material.h"
#include "scene/GameObject.h"
#include "graphics/ShaderProgram.h"
#include "graphics/RenderStage.h"

template <typename Base>
CResourceManager<Base>::CResourceManager ()
{
    static_assert (std::is_base_of<CResource, Base>::value, "Resource base object must inherit from CResource");
    this->m_nextHandle = 0;
    LogMessage (MSG_LOCATION) << "ResourceManager started";
}

template <typename Base>
CResourceManager<Base>::~CResourceManager ()
{
    LogMessage (MSG_LOCATION) << "ResourceManager stopped";
}

template <typename Base>
typename CResourceManager<Base>::iterator CResourceManager<Base>::begin ()
{ 
    return m_resources.begin (); 
}

template <typename Base>
typename CResourceManager<Base>::iterator CResourceManager<Base>::end () 
{ 
    return m_resources.end (); 
}

template <typename Base>
typename CResourceManager<Base>::const_iterator CResourceManager<Base>::begin () const 
{ 
    return m_resources.begin (); 
}

template <typename Base>
typename CResourceManager<Base>::const_iterator CResourceManager<Base>::end () const 
{ 
    return m_resources.end (); 
}

template <typename Base>
typename CResourceManager<Base>::const_iterator CResourceManager<Base>::cbegin () const 
{ 
    return m_resources.cbegin (); 
}

template <typename Base>
typename CResourceManager<Base>::const_iterator CResourceManager<Base>::cend () const 
{ 
    return m_resources.cend (); 
}

template <typename Base>
std::shared_ptr<Base> CResourceManager<Base>::Push (const std::string& name, std::shared_ptr<Base> resource)
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

template __EAPI CResourceManager<CResource>::CResourceManager ();
template __EAPI CResourceManager<CResource>::~CResourceManager ();
template __EAPI CResourceManager<CResource>::iterator CResourceManager<CResource>::begin ();
template __EAPI CResourceManager<CResource>::iterator CResourceManager<CResource>::end ();
template __EAPI CResourceManager<CResource>::const_iterator CResourceManager<CResource>::begin () const;
template __EAPI CResourceManager<CResource>::const_iterator CResourceManager<CResource>::end () const;
template __EAPI CResourceManager<CResource>::const_iterator CResourceManager<CResource>::cbegin () const;
template __EAPI CResourceManager<CResource>::const_iterator CResourceManager<CResource>::cend () const;
template __EAPI std::shared_ptr<CResource> CResourceManager<CResource>::Push (const std::string& name, std::shared_ptr<CResource> resource);

template __EAPI CResourceManager<CGameScene>::CResourceManager ();
template __EAPI CResourceManager<CGameScene>::~CResourceManager ();
template __EAPI CResourceManager<CGameScene>::iterator CResourceManager<CGameScene>::begin ();
template __EAPI CResourceManager<CGameScene>::iterator CResourceManager<CGameScene>::end ();
template __EAPI CResourceManager<CGameScene>::const_iterator CResourceManager<CGameScene>::begin () const;
template __EAPI CResourceManager<CGameScene>::const_iterator CResourceManager<CGameScene>::end () const;
template __EAPI CResourceManager<CGameScene>::const_iterator CResourceManager<CGameScene>::cbegin () const;
template __EAPI CResourceManager<CGameScene>::const_iterator CResourceManager<CGameScene>::cend () const;
template __EAPI std::shared_ptr<CGameScene> CResourceManager<CGameScene>::Push (const std::string& name, std::shared_ptr<CGameScene> resource);

template __EAPI CResourceManager<Material>::CResourceManager ();
template __EAPI CResourceManager<Material>::~CResourceManager ();
template __EAPI CResourceManager<Material>::iterator CResourceManager<Material>::begin ();
template __EAPI CResourceManager<Material>::iterator CResourceManager<Material>::end ();
template __EAPI CResourceManager<Material>::const_iterator CResourceManager<Material>::begin () const;
template __EAPI CResourceManager<Material>::const_iterator CResourceManager<Material>::end () const;
template __EAPI CResourceManager<Material>::const_iterator CResourceManager<Material>::cbegin () const;
template __EAPI CResourceManager<Material>::const_iterator CResourceManager<Material>::cend () const;
template __EAPI std::shared_ptr<Material> CResourceManager<Material>::Push (const std::string& name, std::shared_ptr<Material> resource);

template __EAPI CResourceManager<GameObject>::CResourceManager ();
template __EAPI CResourceManager<GameObject>::~CResourceManager ();
template __EAPI CResourceManager<GameObject>::iterator CResourceManager<GameObject>::begin ();
template __EAPI CResourceManager<GameObject>::iterator CResourceManager<GameObject>::end ();
template __EAPI CResourceManager<GameObject>::const_iterator CResourceManager<GameObject>::begin () const;
template __EAPI CResourceManager<GameObject>::const_iterator CResourceManager<GameObject>::end () const;
template __EAPI CResourceManager<GameObject>::const_iterator CResourceManager<GameObject>::cbegin () const;
template __EAPI CResourceManager<GameObject>::const_iterator CResourceManager<GameObject>::cend () const;
template __EAPI std::shared_ptr<GameObject> CResourceManager<GameObject>::Push (const std::string& name, std::shared_ptr<GameObject> resource);

template __EAPI CResourceManager<ShaderProgram>::CResourceManager ();
template __EAPI CResourceManager<ShaderProgram>::~CResourceManager ();
template __EAPI CResourceManager<ShaderProgram>::iterator CResourceManager<ShaderProgram>::begin ();
template __EAPI CResourceManager<ShaderProgram>::iterator CResourceManager<ShaderProgram>::end ();
template __EAPI CResourceManager<ShaderProgram>::const_iterator CResourceManager<ShaderProgram>::begin () const;
template __EAPI CResourceManager<ShaderProgram>::const_iterator CResourceManager<ShaderProgram>::end () const;
template __EAPI CResourceManager<ShaderProgram>::const_iterator CResourceManager<ShaderProgram>::cbegin () const;
template __EAPI CResourceManager<ShaderProgram>::const_iterator CResourceManager<ShaderProgram>::cend () const;
template __EAPI std::shared_ptr<ShaderProgram> CResourceManager<ShaderProgram>::Push (const std::string& name, std::shared_ptr<ShaderProgram> resource);

template __EAPI CResourceManager<CRenderStage>::CResourceManager ();
template __EAPI CResourceManager<CRenderStage>::~CResourceManager ();
template __EAPI CResourceManager<CRenderStage>::iterator CResourceManager<CRenderStage>::begin ();
template __EAPI CResourceManager<CRenderStage>::iterator CResourceManager<CRenderStage>::end ();
template __EAPI CResourceManager<CRenderStage>::const_iterator CResourceManager<CRenderStage>::begin () const;
template __EAPI CResourceManager<CRenderStage>::const_iterator CResourceManager<CRenderStage>::end () const;
template __EAPI CResourceManager<CRenderStage>::const_iterator CResourceManager<CRenderStage>::cbegin () const;
template __EAPI CResourceManager<CRenderStage>::const_iterator CResourceManager<CRenderStage>::cend () const;
template __EAPI std::shared_ptr<CRenderStage> CResourceManager<CRenderStage>::Push (const std::string& name, std::shared_ptr<CRenderStage> resource);