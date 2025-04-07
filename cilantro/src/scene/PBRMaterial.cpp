#include "cilantroengine.h"
#include "scene/PBRMaterial.h"
#include "resource/ResourceManager.h"
#include "graphics/Renderer.h"
#include "system/Game.h"
#include "math/Vector3f.h"

namespace cilantro
{

PBRMaterial::PBRMaterial (std::shared_ptr<GameScene> scene) : Material (scene)
{
    m_forwardShaderProgram = "pbr_forward_shader";
    m_deferredGeometryPassShaderProgram = "pbr_deferred_geometrypass_shader";
    m_deferredLightingPassShaderProgram = "pbr_deferred_lightingpass_shader";
    
    m_albedo = std::make_shared<Texture> (1, 1, Vector3f (1.0f, 1.0f, 1.0f));
    m_normal = std::make_shared<Texture> (1, 1, Vector3f (0.5f, 0.5f, 1.0f));
    m_metallic = std::make_shared<Texture> (1, 1, 0.0f);
    m_roughness = std::make_shared<Texture> (1, 1, 1.0f);
    m_ao = std::make_shared<Texture> (1, 1, 1.0f);

    m_textures[static_cast<int>(PBRTexture::Albedo)] = std::pair ("tAlbedo", m_albedo);
    m_textures[static_cast<int>(PBRTexture::Normal)] = std::pair ("tNormal", m_normal);
    m_textures[static_cast<int>(PBRTexture::Metallic)] = std::pair ("tMetallic", m_metallic);
    m_textures[static_cast<int>(PBRTexture::Roughness)] = std::pair ("tRoughness", m_roughness);
    m_textures[static_cast<int>(PBRTexture::AO)] = std::pair ("tAO", m_ao);
}

PBRMaterial::~PBRMaterial ()
{
}

std::shared_ptr<PBRMaterial> PBRMaterial::SetAlbedo (const std::string& albedo)
{
    auto tAlbedo = GetGameScene ()->GetGame ()->GetResourceManager ().GetByName<Texture> (albedo);
    SetTexture (static_cast<int>(PBRTexture::Albedo), "tAlbedo", tAlbedo);

    return std::dynamic_pointer_cast<PBRMaterial> (shared_from_this ());
}

std::shared_ptr<PBRMaterial> PBRMaterial::SetNormal (const std::string& normal)
{
    auto tNormal = GetGameScene ()->GetGame ()->GetResourceManager ().GetByName<Texture> (normal);
    SetTexture (static_cast<int>(PBRTexture::Normal), "tNormal", tNormal);

    return std::dynamic_pointer_cast<PBRMaterial> (shared_from_this ());
}

std::shared_ptr<PBRMaterial> PBRMaterial::SetMetallic (const std::string& metallic)
{
    auto tMetallic = GetGameScene ()->GetGame ()->GetResourceManager ().GetByName<Texture> (metallic);
    SetTexture (static_cast<int>(PBRTexture::Metallic), "tMetallic", tMetallic);

    return std::dynamic_pointer_cast<PBRMaterial> (shared_from_this ());
}

std::shared_ptr<PBRMaterial> PBRMaterial::SetRoughness (const std::string& roughness)
{
    auto tRoughness = GetGameScene ()->GetGame ()->GetResourceManager ().GetByName<Texture> (roughness);
    SetTexture (static_cast<int>(PBRTexture::Roughness), "tRoughness", tRoughness);

    return std::dynamic_pointer_cast<PBRMaterial> (shared_from_this ());
}

std::shared_ptr<PBRMaterial> PBRMaterial::SetAO (const std::string& ao)
{
    auto tAO = GetGameScene ()->GetGame ()->GetResourceManager ().GetByName<Texture> (ao);
    SetTexture (static_cast<int>(PBRTexture::AO), "tAO", tAO);

    return std::dynamic_pointer_cast<PBRMaterial> (shared_from_this ());
}

std::shared_ptr<PBRMaterial> PBRMaterial::SetAlbedo (const Vector3f& albedo)
{
    m_albedo->GenerateSolid (1, 1, albedo);
    SetTexture (static_cast<int>(PBRTexture::Albedo), "tAlbedo", m_albedo);

    return std::dynamic_pointer_cast<PBRMaterial> (shared_from_this ());
}

std::shared_ptr<PBRMaterial> PBRMaterial::SetMetallic (const float metallic)
{
    m_metallic->GenerateSolid (1, 1, metallic);
    SetTexture (static_cast<int>(PBRTexture::Metallic), "tMetallic", m_metallic);

    return std::dynamic_pointer_cast<PBRMaterial> (shared_from_this ());
}

std::shared_ptr<PBRMaterial> PBRMaterial::SetRoughness (const float roughness)
{
    m_roughness->GenerateSolid (1, 1, roughness);
    SetTexture (static_cast<int>(PBRTexture::Roughness), "tRoughness", m_roughness);

    return std::dynamic_pointer_cast<PBRMaterial> (shared_from_this ());
}

std::shared_ptr<PBRMaterial> PBRMaterial::SetAO (const float ao)
{
    m_ao->GenerateSolid (1, 1, ao);
    SetTexture (static_cast<int>(PBRTexture::AO), "tAO", m_ao);

    return std::dynamic_pointer_cast<PBRMaterial> (shared_from_this ());
}

std::shared_ptr<Texture> PBRMaterial::GetAlbedo ()
{
    return m_textures[static_cast<int>(PBRTexture::Albedo)].second;
}

std::shared_ptr<Texture> PBRMaterial::GetNormal ()
{
    return m_textures[static_cast<int>(PBRTexture::Normal)].second;
}

std::shared_ptr<Texture> PBRMaterial::GetMetallic ()
{
    return m_textures[static_cast<int>(PBRTexture::Metallic)].second;
}

std::shared_ptr<Texture> PBRMaterial::GetRoughness ()
{
    return m_textures[static_cast<int>(PBRTexture::Roughness)].second;
}

std::shared_ptr<Texture> PBRMaterial::GetAO ()
{
    return m_textures[static_cast<int>(PBRTexture::AO)].second;
}

} // namespace cilantro