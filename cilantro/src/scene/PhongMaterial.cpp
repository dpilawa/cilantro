#include "cilantroengine.h"
#include "scene/PhongMaterial.h"
#include "resource/ResourceManager.h"
#include "graphics/Renderer.h"
#include "math/Vector3f.h"

PhongMaterial::PhongMaterial () : Material ()
{
    SetShaderProgram ("blinnphong_shader");

    diffuse.GenerateSolid (1, 1, Vector3f (1.0f, 1.0f, 1.0f));
    normal.GenerateSolid (1, 1, Vector3f (0.0f, 0.0f, 0.0f));
    specular.GenerateSolid (1, 1, Vector3f (1.0f, 1.0f, 1.0f));
    emissive.GenerateSolid (1, 1, Vector3f (0.0f, 0.0f, 0.0f));

    SetSpecularShininess (32.0f);

    SetTexture (static_cast<int>(PhongTexture::Diffuse), "tDiffuse", diffuse);
    SetTexture (static_cast<int>(PhongTexture::Normal), "tNormal", normal);
    SetTexture (static_cast<int>(PhongTexture::Specular), "tSpecular", specular);
    SetTexture (static_cast<int>(PhongTexture::Emissive), "tEmissive", emissive);
}

PhongMaterial::~PhongMaterial ()
{
}

PhongMaterial& PhongMaterial::SetDiffuse (unsigned int hDiffuse)
{
    Texture& tDiffuse = game->GetResourceManager ().GetByHandle<Texture> (hDiffuse);
    SetTexture (static_cast<int>(PhongTexture::Diffuse), "tDiffuse", tDiffuse);

    return *this;
}

PhongMaterial& PhongMaterial::SetNormal (unsigned int hNormal)
{
    Texture& tNormal = game->GetResourceManager ().GetByHandle<Texture> (hNormal);
    SetTexture (static_cast<int>(PhongTexture::Normal), "tNormal", tNormal);

    return *this;
}

PhongMaterial& PhongMaterial::SetSpecular (unsigned int hSpecular)
{
    Texture& tSpecular = game->GetResourceManager ().GetByHandle<Texture> (hSpecular);
    SetTexture (static_cast<int>(PhongTexture::Specular), "tSpecular", tSpecular);

    return *this;
}

PhongMaterial& PhongMaterial::SetEmissive (unsigned int hEmissive)
{
    Texture& tEmissive = game->GetResourceManager ().GetByHandle<Texture> (hEmissive);
    SetTexture (static_cast<int>(PhongTexture::Emissive), "tEmissive", tEmissive);
    
    return *this;
}

PhongMaterial& PhongMaterial::SetDiffuse (const Vector3f& diffuse)
{
    this->diffuse.GenerateSolid (1, 1, diffuse);
    SetTexture (static_cast<int>(PhongTexture::Diffuse), "tDiffuse", this->diffuse);

    return *this;
}

PhongMaterial& PhongMaterial::SetSpecular (const Vector3f& specular)
{
    this->specular.GenerateSolid (1, 1, specular);
    SetTexture (static_cast<int>(PhongTexture::Specular), "tSpecular", this->specular);

    return *this;
}

PhongMaterial& PhongMaterial::SetSpecularShininess (float specularShininess)
{
    SetProperty ("fSpecularShininess", specularShininess);

    return *this;
}

PhongMaterial& PhongMaterial::SetEmissive (const Vector3f& emissive)
{
    this->emissive.GenerateSolid (1, 1, emissive);
    SetTexture (static_cast<int>(PhongTexture::Emissive), "tEmissive", this->emissive);

    return *this;
}

Texture& PhongMaterial::GetDiffuse ()
{
    return *(textures[static_cast<int>(PhongTexture::Diffuse)].second);
}

Texture& PhongMaterial::GetNormal ()
{
    return *(textures[static_cast<int>(PhongTexture::Normal)].second);
}

Texture& PhongMaterial::GetSpecular ()
{
    return *(textures[static_cast<int>(PhongTexture::Specular)].second);
}

float PhongMaterial::GetSpecularShininess ()
{
    return properties["tSpecularShininess"][0];
}

Texture& PhongMaterial::GetEmissive ()
{
    return *(textures[static_cast<int>(PhongTexture::Emissive)].second);
}

