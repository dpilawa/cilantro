#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include "graphics/Renderer.h"
#include "scene/PBRMaterial.h"
#include "math/Vector3f.h"

PBRMaterial::PBRMaterial () : albedo ("albedo"), normal ("normal"), metallic ("metallic"), roughness ("roughness"), ao ("ao")
{
    SetShaderProgram ("pbr_shader");

    albedo.GenerateSolid (1, 1, Vector3f (1.0f, 1.0f, 1.0f));
    normal.GenerateSolid (1, 1, Vector3f (0.0f, 0.0f, 0.0f));
    metallic.GenerateSolid (1, 1, 0.0f);
    roughness.GenerateSolid (1, 1, 1.0f);
    ao.GenerateSolid (1, 1, 1.0f);

    SetTexture (static_cast<int>(PBRTexture::Albedo), "tAlbedo", &albedo);
    SetTexture (static_cast<int>(PBRTexture::Normal), "tNormal", &normal);
    SetTexture (static_cast<int>(PBRTexture::Metallic), "tMetallic", &metallic);
    SetTexture (static_cast<int>(PBRTexture::Roughness), "tRoughness", &roughness);
    SetTexture (static_cast<int>(PBRTexture::AO), "tAO", &ao);
}

PBRMaterial::~PBRMaterial ()
{
}

PBRMaterial& PBRMaterial::SetAlbedo (unsigned int hAlbedo)
{
    auto tAlbedo = game->GetResourceManager ().GetByHandle<Texture> (hAlbedo).get ();
    SetTexture (static_cast<int>(PBRTexture::Albedo), "tAlbedo", tAlbedo);

    return *this;
}

PBRMaterial& PBRMaterial::SetNormal (unsigned int hNormal)
{
    auto tNormal = game->GetResourceManager ().GetByHandle<Texture> (hNormal).get ();
    SetTexture (static_cast<int>(PBRTexture::Normal), "tNormal", tNormal);

    return *this;
}

PBRMaterial& PBRMaterial::SetMetallic (unsigned int hMetallic)
{
    auto tMetallic = game->GetResourceManager ().GetByHandle<Texture> (hMetallic).get ();
    SetTexture (static_cast<int>(PBRTexture::Metallic), "tMetallic", tMetallic);

    return *this;
}

PBRMaterial& PBRMaterial::SetRoughness (unsigned int hRoughness)
{
    auto tRoughness = game->GetResourceManager ().GetByHandle<Texture> (hRoughness).get ();
    SetTexture (static_cast<int>(PBRTexture::Roughness), "tRoughness", tRoughness);

    return *this;
}

PBRMaterial& PBRMaterial::SetAO (unsigned int hAo)
{
    auto tAO = game->GetResourceManager ().GetByHandle<Texture> (hAo).get ();
    SetTexture (static_cast<int>(PBRTexture::AO), "tAO", tAO);

    return *this;
}

PBRMaterial& PBRMaterial::SetAlbedo (const Vector3f& albedo)
{
    this->albedo.GenerateSolid (1, 1, albedo);
    SetTexture (static_cast<int>(PBRTexture::Albedo), "tAlbedo", &this->albedo);

    return *this;
}

PBRMaterial& PBRMaterial::SetMetallic (const float metallic)
{
    this->metallic.GenerateSolid (1, 1, metallic);
    SetTexture (static_cast<int>(PBRTexture::Metallic), "tMetallic", &this->metallic);

    return *this;
}

PBRMaterial& PBRMaterial::SetRoughness (const float roughness)
{
    this->roughness.GenerateSolid (1, 1, roughness);
    SetTexture (static_cast<int>(PBRTexture::Roughness), "tRoughness", &this->roughness);

    return *this;
}

PBRMaterial& PBRMaterial::SetAO (const float ao)
{
    this->ao.GenerateSolid (1, 1, ao);
    SetTexture (static_cast<int>(PBRTexture::AO), "tAO", &this->ao);

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