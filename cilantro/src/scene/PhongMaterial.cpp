#include "cilantroengine.h"
#include "scene/PhongMaterial.h"
#include "resource/ResourceManager.h"
#include "graphics/Renderer.h"
#include "system/Game.h"
#include "math/Vector3f.h"

namespace cilantro
{

PhongMaterial::PhongMaterial (std::shared_ptr<GameScene> scene) : Material (scene)
{
    m_forwardShaderProgram = "blinnphong_forward_shader";
    m_deferredGeometryPassShaderProgram = "blinnphong_deferred_geometrypass_shader";
    m_deferredLightingPassShaderProgram = "blinnphong_deferred_lightingpass_shader";

    m_properties["fSpecularShininess"] = {32.0f};

    m_diffuse = std::make_shared<Texture> (1, 1, Vector3f (1.0f, 1.0f, 1.0f));
    m_normal = std::make_shared<Texture> (1, 1, Vector3f (0.5f, 0.5f, 1.0f));
    m_specular = std::make_shared<Texture> (1, 1, Vector3f (1.0f, 1.0f, 1.0f));
    m_emissive = std::make_shared<Texture> (1, 1, Vector3f (0.0f, 0.0f, 0.0f));

    m_textures[static_cast<int>(PhongTexture::Diffuse)] = std::pair ("tDiffuse", m_diffuse);
    m_textures[static_cast<int>(PhongTexture::Normal)] = std::pair ("tNormal", m_normal);
    m_textures[static_cast<int>(PhongTexture::Specular)] = std::pair ("tSpecular", m_specular);
    m_textures[static_cast<int>(PhongTexture::Emissive)] = std::pair ("tEmissive", m_emissive);
}

PhongMaterial::~PhongMaterial ()
{
}

PhongMaterial& PhongMaterial::SetDiffuse (const std::string& diffuse)
{
    auto tDiffuse = GetGameScene ()->GetGame ()->GetResourceManager ().GetByName<Texture> (diffuse);
    SetTexture (static_cast<int>(PhongTexture::Diffuse), "tDiffuse", tDiffuse);

    return *this;
}

PhongMaterial& PhongMaterial::SetNormal (const std::string& normal)
{
    auto tNormal = GetGameScene ()->GetGame ()->GetResourceManager ().GetByName<Texture> (normal);
    SetTexture (static_cast<int>(PhongTexture::Normal), "tNormal", tNormal);

    return *this;
}

PhongMaterial& PhongMaterial::SetSpecular (const std::string& specular)
{
    auto tSpecular = GetGameScene ()->GetGame ()->GetResourceManager ().GetByName<Texture> (specular);
    SetTexture (static_cast<int>(PhongTexture::Specular), "tSpecular", tSpecular);

    return *this;
}

PhongMaterial& PhongMaterial::SetEmissive (const std::string& emissive)
{
    auto tEmissive = GetGameScene ()->GetGame ()->GetResourceManager ().GetByName<Texture> (emissive);
    SetTexture (static_cast<int>(PhongTexture::Emissive), "tEmissive", tEmissive);
    
    return *this;
}

PhongMaterial& PhongMaterial::SetDiffuse (const Vector3f& diffuse)
{
    m_diffuse->GenerateSolid (1, 1, diffuse);
    SetTexture (static_cast<int>(PhongTexture::Diffuse), "tDiffuse", m_diffuse);

    return *this;
}

PhongMaterial& PhongMaterial::SetSpecular (const Vector3f& specular)
{
    m_specular->GenerateSolid (1, 1, specular);
    SetTexture (static_cast<int>(PhongTexture::Specular), "tSpecular", m_specular);

    return *this;
}

PhongMaterial& PhongMaterial::SetSpecularShininess (float specularShininess)
{
    SetProperty ("fSpecularShininess", specularShininess);

    return *this;
}

PhongMaterial& PhongMaterial::SetEmissive (const Vector3f& emissive)
{
    m_emissive->GenerateSolid (1, 1, emissive);
    SetTexture (static_cast<int>(PhongTexture::Emissive), "tEmissive", m_emissive);

    return *this;
}

std::shared_ptr<Texture> PhongMaterial::GetDiffuse ()
{
    return m_textures[static_cast<int>(PhongTexture::Diffuse)].second;
}

std::shared_ptr<Texture> PhongMaterial::GetNormal ()
{
    return m_textures[static_cast<int>(PhongTexture::Normal)].second;
}

std::shared_ptr<Texture> PhongMaterial::GetSpecular ()
{
    return m_textures[static_cast<int>(PhongTexture::Specular)].second;
}

float PhongMaterial::GetSpecularShininess ()
{
    return m_properties["tSpecularShininess"][0];
}

std::shared_ptr<Texture> PhongMaterial::GetEmissive ()
{
    return m_textures[static_cast<int>(PhongTexture::Emissive)].second;
}

} // namespace cilantro