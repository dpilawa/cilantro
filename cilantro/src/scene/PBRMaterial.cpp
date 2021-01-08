#include "cilantroengine.h"
#include "graphics/Renderer.h"
#include "scene/PBRMaterial.h"
#include "math/Vector3f.h"

PBRMaterial::PBRMaterial ()
{
    SetShaderProgram ("pbr_shader");

    albedo.GenerateSolid (1, 1, Vector3f (1.0f, 1.0f, 1.0f));
    normal.GenerateSolid (1, 1, Vector3f (0.0f, 0.0f, 0.0f));
    metallic.GenerateSolid (1, 1, 0.0f);
    roughness.GenerateSolid (1, 1, 1.0f);
    ao.GenerateSolid (1, 1, 1.0f);

    SetAlbedo (&albedo);
    SetNormal (&normal);
    SetMetallic (&metallic);
    SetRoughness (&roughness);
    SetAO (&ao);
}

PBRMaterial::~PBRMaterial ()
{
}

void PBRMaterial::OnUpdate (Renderer& renderer, unsigned int textureUnit)
{
    renderer.Update (*this, textureUnit);
}

PBRMaterial& PBRMaterial::SetAlbedo (Texture* albedo)
{
    Material::SetTexture (static_cast<int>(PBRTexture::Albedo), "tAlbedo", albedo);

    return *this;
}

PBRMaterial& PBRMaterial::SetNormal (Texture* normal)
{
    Material::SetTexture (static_cast<int>(PBRTexture::Normal), "tNormal", normal);

    return *this;
}

PBRMaterial& PBRMaterial::SetMetallic (Texture* metallic)
{
    Material::SetTexture (static_cast<int>(PBRTexture::Metallic), "tMetallic", metallic);

    return *this;
}

PBRMaterial& PBRMaterial::SetRoughness (Texture* roughness)
{
    Material::SetTexture (static_cast<int>(PBRTexture::Roughness), "tRoughness", roughness);

    return *this;
}

PBRMaterial& PBRMaterial::SetAO (Texture* ao)
{
    Material::SetTexture (static_cast<int>(PBRTexture::AO), "tAO", ao);

    return *this;
}

PBRMaterial& PBRMaterial::SetAlbedo (const Vector3f& albedo)
{
    this->albedo.GenerateSolid (1, 1, albedo);
    PBRMaterial::SetAlbedo (&this->albedo);

    return *this;
}

PBRMaterial& PBRMaterial::SetMetallic (const float metallic)
{
    this->metallic.GenerateSolid (1, 1, metallic);
    PBRMaterial::SetMetallic (&this->metallic);

    return *this;
}

PBRMaterial& PBRMaterial::SetRoughness (const float roughness)
{
    this->roughness.GenerateSolid (1, 1, roughness);
    PBRMaterial::SetRoughness (&this->roughness);

    return *this;
}

PBRMaterial& PBRMaterial::SetAO (const float ao)
{
    this->ao.GenerateSolid (1, 1, ao);
    PBRMaterial::SetAO (&this->ao);

    return *this;
}

Texture* PBRMaterial::GetAlbedo ()
{
    return textures[static_cast<int>(PBRTexture::Albedo)].second;
}

Texture* PBRMaterial::GetNormal ()
{
    return textures[static_cast<int>(PBRTexture::Normal)].second;
}

Texture* PBRMaterial::GetMetallic ()
{
    return textures[static_cast<int>(PBRTexture::Metallic)].second;
}

Texture* PBRMaterial::GetRoughness ()
{
    return textures[static_cast<int>(PBRTexture::Roughness)].second;
}

Texture* PBRMaterial::GetAO ()
{
    return textures[static_cast<int>(PBRTexture::AO)].second;
}