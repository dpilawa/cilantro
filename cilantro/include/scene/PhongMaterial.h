#ifndef _PHONGMATERIAL_H_
#define _PHONGMATERIAL_H_

#include "cilantroengine.h"
#include "scene/Material.h"
#include "resource/Texture.h"
#include "math/Vector3f.h"

struct IRenderer;

enum class PhongTexture : int
{
    Diffuse = 0,
    Normal,
    Specular,
    Emissive
};

class PhongMaterial : public Material
{
public:
    __EAPI PhongMaterial ();
    __EAPI virtual ~PhongMaterial ();

    // setters
    __EAPI PhongMaterial& SetDiffuse (const std::string& diffuse);
    __EAPI PhongMaterial& SetNormal (const std::string& normal);
    __EAPI PhongMaterial& SetSpecular (const std::string& specular);
    __EAPI PhongMaterial& SetEmissive (const std::string& emissive);

    __EAPI PhongMaterial& SetDiffuse (const Vector3f& diffuse);
    __EAPI PhongMaterial& SetSpecular (const Vector3f& specular);
    __EAPI PhongMaterial& SetSpecularShininess (const float specularShininess);
    __EAPI PhongMaterial& SetEmissive (const Vector3f& emissive);


    // getters
    __EAPI Texture& GetDiffuse ();
    __EAPI Texture& GetNormal ();
    __EAPI Texture& GetSpecular ();
    __EAPI float GetSpecularShininess ();
    __EAPI Texture& GetEmissive ();

protected:

    // static textures
    Texture diffuse {1, 1, Vector3f (1.0f, 1.0f, 1.0f)};
    Texture normal {1, 1, Vector3f (0.5f, 0.5f, 1.0f)};
    Texture specular {1, 1, Vector3f (1.0f, 1.0f, 1.0f)};
    Texture emissive {1, 1, Vector3f (0.0f, 0.0f, 0.0f)};

};

#endif