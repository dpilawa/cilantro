#ifndef _PBRMATERIAL_H_
#define _PBRMATERIAL_H_

#include "cilantroengine.h"
#include "scene/Material.h"
#include "resource/Texture.h"
#include "math/Vector3f.h"

struct IRenderer;

namespace cilantro {

enum class PBRTexture : int
{
    Albedo = 0,
    Normal,
    Metallic,
    Roughness,
    AO
};

class __CEAPI PBRMaterial : public Material
{
public:
    __EAPI PBRMaterial (std::shared_ptr<GameScene> scene);
    __EAPI virtual ~PBRMaterial ();

    // setters
    __EAPI PBRMaterial& SetAlbedo (const std::string& albedo);
    __EAPI PBRMaterial& SetNormal (const std::string& normal);
    __EAPI PBRMaterial& SetMetallic (const std::string& metallic);
    __EAPI PBRMaterial& SetRoughness (const std::string& roughness);
    __EAPI PBRMaterial& SetAO (const std::string& ao);

    __EAPI PBRMaterial& SetAlbedo (const Vector3f& albedo);
    __EAPI PBRMaterial& SetMetallic (const float metallic);
    __EAPI PBRMaterial& SetRoughness (const float roughness);
    __EAPI PBRMaterial& SetAO (const float ao);

    // getters
    __EAPI std::shared_ptr<Texture> GetAlbedo ();
    __EAPI std::shared_ptr<Texture> GetNormal ();
    __EAPI std::shared_ptr<Texture> GetMetallic ();
    __EAPI std::shared_ptr<Texture> GetRoughness ();
    __EAPI std::shared_ptr<Texture> GetAO ();

protected:

    // static textures
    std::shared_ptr<Texture> m_albedo;
    std::shared_ptr<Texture> m_normal;
    std::shared_ptr<Texture> m_metallic;
    std::shared_ptr<Texture> m_roughness;
    std::shared_ptr<Texture> m_ao;

};

} // namespace cilantro

#endif