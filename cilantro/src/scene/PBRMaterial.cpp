#include "cilantroengine.h"
#include "scene/PBRMaterial.h"
#include "resource/ResourceManager.h"
#include "graphics/Renderer.h"
#include "system/EngineContext.h"
#include "math/Vector3f.h"


PBRMaterial::PBRMaterial () : Material ()
{
    SetShaderProgram ("pbr_shader");

    SetTexture (static_cast<int>(PBRTexture::Albedo), "tAlbedo", albedo);
    SetTexture (static_cast<int>(PBRTexture::Normal), "tNormal", normal);
    SetTexture (static_cast<int>(PBRTexture::Metallic), "tMetallic", metallic);
    SetTexture (static_cast<int>(PBRTexture::Roughness), "tRoughness", roughness);
    SetTexture (static_cast<int>(PBRTexture::AO), "tAO", ao);
}

PBRMaterial::~PBRMaterial ()
{
}

PBRMaterial& PBRMaterial::SetAlbedo (const std::string& albedo)
{
    Texture& tAlbedo = EngineContext::GetResourceManager ().GetByName<Texture> (albedo);
    SetTexture (static_cast<int>(PBRTexture::Albedo), "tAlbedo", tAlbedo);

    return *this;
}

PBRMaterial& PBRMaterial::SetNormal (const std::string& normal)
{
    Texture& tNormal = EngineContext::GetResourceManager ().GetByName<Texture> (normal);
    SetTexture (static_cast<int>(PBRTexture::Normal), "tNormal", tNormal);

    return *this;
}

PBRMaterial& PBRMaterial::SetMetallic (const std::string& metallic)
{
    Texture& tMetallic = EngineContext::GetResourceManager ().GetByName<Texture> (metallic);
    SetTexture (static_cast<int>(PBRTexture::Metallic), "tMetallic", tMetallic);

    return *this;
}

PBRMaterial& PBRMaterial::SetRoughness (const std::string& roughness)
{
    Texture& tRoughness = EngineContext::GetResourceManager ().GetByName<Texture> (roughness);
    SetTexture (static_cast<int>(PBRTexture::Roughness), "tRoughness", tRoughness);

    return *this;
}

PBRMaterial& PBRMaterial::SetAO (const std::string& ao)
{
    Texture& tAO = EngineContext::GetResourceManager ().GetByName<Texture> (ao);
    SetTexture (static_cast<int>(PBRTexture::AO), "tAO", tAO);

    return *this;
}

PBRMaterial& PBRMaterial::SetAlbedo (const Vector3f& albedo)
{
    this->albedo.GenerateSolid (1, 1, albedo);
    SetTexture (static_cast<int>(PBRTexture::Albedo), "tAlbedo", this->albedo);

    return *this;
}

PBRMaterial& PBRMaterial::SetMetallic (const float metallic)
{
    this->metallic.GenerateSolid (1, 1, metallic);
    SetTexture (static_cast<int>(PBRTexture::Metallic), "tMetallic", this->metallic);

    return *this;
}

PBRMaterial& PBRMaterial::SetRoughness (const float roughness)
{
    this->roughness.GenerateSolid (1, 1, roughness);
    SetTexture (static_cast<int>(PBRTexture::Roughness), "tRoughness", this->roughness);

    return *this;
}

PBRMaterial& PBRMaterial::SetAO (const float ao)
{
    this->ao.GenerateSolid (1, 1, ao);
    SetTexture (static_cast<int>(PBRTexture::AO), "tAO", this->ao);

    return *this;
}

Texture& PBRMaterial::GetAlbedo ()
{
    return *(textures[static_cast<int>(PBRTexture::Albedo)].second);
}

Texture& PBRMaterial::GetNormal ()
{
    return *(textures[static_cast<int>(PBRTexture::Normal)].second);
}

Texture& PBRMaterial::GetMetallic ()
{
    return *(textures[static_cast<int>(PBRTexture::Metallic)].second);
}

Texture& PBRMaterial::GetRoughness ()
{
    return *(textures[static_cast<int>(PBRTexture::Roughness)].second);
}

Texture& PBRMaterial::GetAO ()
{
    return *(textures[static_cast<int>(PBRTexture::AO)].second);
}