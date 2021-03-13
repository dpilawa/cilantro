#ifndef _PBRMATERIAL_H_
#define _PBRMATERIAL_H_

#include "cilantroengine.h"
#include "scene/Material.h"
#include "resource/Texture.h"
#include "math/Vector3f.h"

class Renderer;

enum class PBRTexture : int
{
    Albedo = 0,
    Normal,
    Metallic,
    Roughness,
    AO
};

class PBRMaterial : public Material
{
public:
    __EAPI PBRMaterial ();
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
    __EAPI Texture& GetAlbedo ();
    __EAPI Texture& GetNormal ();
    __EAPI Texture& GetMetallic ();
    __EAPI Texture& GetRoughness ();
    __EAPI Texture& GetAO ();

protected:

    // static textures
    Texture albedo {1, 1, Vector3f (1.0f, 1.0f, 1.0f)};
    Texture normal {1, 1, Vector3f (0.0f, 0.0f, 0.0f)};
    Texture metallic {1, 1, 0.0f};
    Texture roughness {1, 1, 1.0f};
    Texture ao {1, 1, 1.0f};

};

#endif