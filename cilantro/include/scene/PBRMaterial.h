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

    __EAPI virtual void OnUpdate (Renderer &renderer, unsigned int textureUnit = 0);

    // setters
    __EAPI PBRMaterial& SetAlbedo (Texture* albedo);
    __EAPI PBRMaterial& SetNormal (Texture* normal);
    __EAPI PBRMaterial& SetMetallic (Texture* metallic);
    __EAPI PBRMaterial& SetRoughness (Texture* roughness);
    __EAPI PBRMaterial& SetAO (Texture* ao);

    __EAPI PBRMaterial& SetAlbedo (const Vector3f& albedo);
    __EAPI PBRMaterial& SetMetallic (const float metallic);
    __EAPI PBRMaterial& SetRoughness (const float roughness);
    __EAPI PBRMaterial& SetAO (const float ao);

    // getters
    __EAPI Texture* GetAlbedo ();
    __EAPI Texture* GetNormal ();
    __EAPI Texture* GetMetallic ();
    __EAPI Texture* GetRoughness ();
    __EAPI Texture* GetAO ();

protected:

    // static textures
    Texture albedo;
    Texture normal;
    Texture metallic;
    Texture roughness;
    Texture ao;

};

#endif