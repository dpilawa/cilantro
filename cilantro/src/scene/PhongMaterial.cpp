#include "cilantroengine.h"
#include "scene/PhongMaterial.h"
#include "resource/ResourceManager.h"
#include "graphics/Renderer.h"
#include "system/Game.h"
#include "math/Vector3f.h"

PhongMaterial::PhongMaterial () : Material ()
{
    forwardShaderProgram = "blinnphong_forward_shader";
    deferredGeometryPassShaderProgram = "blinnphong_deferred_geometrypass_shader";
    deferredLightingPassShaderProgram = "blinnphong_deferred_lightingpass_shader";

    properties["fSpecularShininess"] = {32.0f};

    textures[static_cast<int>(PhongTexture::Diffuse)] = std::pair ("tDiffuse", &diffuse);
    textures[static_cast<int>(PhongTexture::Normal)] = std::pair ("tNormal", &normal);
    textures[static_cast<int>(PhongTexture::Specular)] = std::pair ("tSpecular", &specular);
    textures[static_cast<int>(PhongTexture::Emissive)] = std::pair ("tEmissive", &emissive);
}

PhongMaterial::~PhongMaterial ()
{
}

PhongMaterial& PhongMaterial::SetDiffuse (const std::string& diffuse)
{
    Texture& tDiffuse = CGame::GetResourceManager ().GetByName<Texture> (diffuse);
    SetTexture (static_cast<int>(PhongTexture::Diffuse), "tDiffuse", tDiffuse);

    return *this;
}

PhongMaterial& PhongMaterial::SetNormal (const std::string& normal)
{
    Texture& tNormal = CGame::GetResourceManager ().GetByName<Texture> (normal);
    SetTexture (static_cast<int>(PhongTexture::Normal), "tNormal", tNormal);

    return *this;
}

PhongMaterial& PhongMaterial::SetSpecular (const std::string& specular)
{
    Texture& tSpecular = CGame::GetResourceManager ().GetByName<Texture> (specular);
    SetTexture (static_cast<int>(PhongTexture::Specular), "tSpecular", tSpecular);

    return *this;
}

PhongMaterial& PhongMaterial::SetEmissive (const std::string& emissive)
{
    Texture& tEmissive = CGame::GetResourceManager ().GetByName<Texture> (emissive);
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

