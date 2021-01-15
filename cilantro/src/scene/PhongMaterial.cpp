#include "cilantroengine.h"
#include "graphics/Renderer.h"
#include "scene/PhongMaterial.h"
#include "math/Vector3f.h"

PhongMaterial::PhongMaterial () : diffuse ("diffuse"), normal ("normal"), specular ("specular"), emissive ("emissive")
{
    SetShaderProgram ("blinnphong_shader");

    diffuse.GenerateSolid (1, 1, Vector3f (1.0f, 1.0f, 1.0f));
    normal.GenerateSolid (1, 1, Vector3f (0.0f, 0.0f, 0.0f));
    specular.GenerateSolid (1, 1, Vector3f (1.0f, 1.0f, 1.0f));
    emissive.GenerateSolid (1, 1, Vector3f (0.0f, 0.0f, 0.0f));

    SetSpecularShininess (32.0f);

    SetDiffuse (&diffuse);
    SetNormal (&normal);
    SetSpecular (&specular);
    SetEmissive (&emissive);
}

PhongMaterial::~PhongMaterial ()
{
}

void PhongMaterial::OnUpdate (Renderer& renderer, unsigned int textureUnit)
{
    renderer.Update (*this, textureUnit);
}

PhongMaterial& PhongMaterial::SetDiffuse (Texture* diffuse)
{
    Material::SetTexture (static_cast<int>(PhongTexture::Diffuse), "tDiffuse", diffuse);

    return *this;
}

PhongMaterial& PhongMaterial::SetNormal (Texture* normal)
{
    Material::SetTexture (static_cast<int>(PhongTexture::Normal), "tNormal", normal);

    return *this;
}

PhongMaterial& PhongMaterial::SetSpecular (Texture* specular)
{
    Material::SetTexture (static_cast<int>(PhongTexture::Specular), "tSpecular", specular);

    return *this;
}

PhongMaterial& PhongMaterial::SetEmissive (Texture* emissive)
{
    Material::SetTexture (static_cast<int>(PhongTexture::Emissive), "tEmissive", emissive);
    
    return *this;
}

PhongMaterial& PhongMaterial::SetDiffuse (const Vector3f& diffuse)
{
    this->diffuse.GenerateSolid (1, 1, diffuse);
    PhongMaterial::SetDiffuse (&this->diffuse);

    return *this;
}

PhongMaterial& PhongMaterial::SetSpecular (const Vector3f& specular)
{
    this->specular.GenerateSolid (1, 1, specular);
    PhongMaterial::SetSpecular (&this->specular);

    return *this;
}

PhongMaterial& PhongMaterial::SetSpecularShininess (float specularShininess)
{
    Material::SetProperty ("fSpecularShininess", specularShininess);

    return *this;
}

PhongMaterial& PhongMaterial::SetEmissive (const Vector3f& emissive)
{
    this->emissive.GenerateSolid (1, 1, emissive);
    PhongMaterial::SetEmissive (&this->emissive);

    return *this;
}

Texture* PhongMaterial::GetDiffuse ()
{
    return textures[static_cast<int>(PhongTexture::Diffuse)].second;
}

Texture* PhongMaterial::GetNormal ()
{
    return textures[static_cast<int>(PhongTexture::Normal)].second;
}

Texture* PhongMaterial::GetSpecular ()
{
    return textures[static_cast<int>(PhongTexture::Specular)].second;
}

float PhongMaterial::GetSpecularShininess ()
{
    return properties["tSpecularShininess"][0];
}

Texture* PhongMaterial::GetEmissive ()
{
    return textures[static_cast<int>(PhongTexture::Emissive)].second;
}

