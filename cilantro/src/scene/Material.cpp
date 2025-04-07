#include "cilantroengine.h"
#include "scene/Material.h"
#include "math/Vector3f.h"
#include "system/Game.h"
#include <string>

namespace cilantro {

Material::Material (std::shared_ptr<GameScene> scene) : Resource (), m_gameScene (scene)
{
}

Material::~Material ()
{
}

std::shared_ptr<Material> Material::SetForwardShaderProgram (const std::string& name)
{
    m_forwardShaderProgram = name;
    GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<MaterialUpdateMessage> (std::make_shared<MaterialUpdateMessage> (this->GetHandle ()));

    return std::dynamic_pointer_cast<Material> (shared_from_this ());
}

std::shared_ptr<Material> Material::SetDeferredGeometryPassShaderProgram (const std::string& name)
{
    m_deferredGeometryPassShaderProgram = name;
    GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<MaterialUpdateMessage> (std::make_shared<MaterialUpdateMessage> (this->GetHandle ()));
    
    return std::dynamic_pointer_cast<Material> (shared_from_this ());
}

std::shared_ptr<Material> Material::SetDeferredLightingPassShaderProgram (const std::string& name)
{
    m_deferredLightingPassShaderProgram = name;
    GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<MaterialUpdateMessage> (std::make_shared<MaterialUpdateMessage> (this->GetHandle ()));

    return std::dynamic_pointer_cast<Material> (shared_from_this ());
}

std::string Material::GetForwardShaderProgram () const
{
    return m_forwardShaderProgram;
}

std::string Material::GetDeferredGeometryPassShaderProgram () const
{
    return m_deferredGeometryPassShaderProgram;
}

std::string Material::GetDeferredLightingPassShaderProgram () const
{
    return m_deferredLightingPassShaderProgram;
}

std::shared_ptr<GameScene> Material::GetGameScene () const
{
    return m_gameScene.lock ();
}

texture_map_t& Material::GetTexturesMap()
{
    return m_textures;
}

property_map_t& Material::GetPropertiesMap ()
{
    return m_properties;
}

std::shared_ptr<Material> Material::SetTexture (unsigned int textureUnit, const std::string& label, std::shared_ptr<Texture> texture)
{
    m_textures[textureUnit] = std::pair (label, texture);
    GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<MaterialTextureUpdateMessage> (std::make_shared<MaterialTextureUpdateMessage> (this->GetHandle (), textureUnit));

    return std::dynamic_pointer_cast<Material> (shared_from_this ());
}

std::shared_ptr<Material> Material::SetProperty (const std::string& propertyName, float propertyValue)
{
    m_properties[propertyName] = {propertyValue};
    GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<MaterialUpdateMessage> (std::make_shared<MaterialUpdateMessage> (this->GetHandle ()));

    return std::dynamic_pointer_cast<Material> (shared_from_this ());
}


std::shared_ptr<Material> Material::SetProperty (const std::string& propertyName, Vector3f propertyValue)
{
    m_properties[propertyName] = {propertyValue[0], propertyValue[1], propertyValue[2]};

    return std::dynamic_pointer_cast<Material> (shared_from_this ());
}

} // namespace cilantro

