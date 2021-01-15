#ifndef _PHONGMATERIAL_H_
#define _PHONGMATERIAL_H_

#include "cilantroengine.h"
#include "scene/Material.h"
#include "resource/Texture.h"
#include "math/Vector3f.h"

class Renderer;

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

    __EAPI virtual void OnUpdate (Renderer &renderer, unsigned int textureUnit = 0);

    // setters
    __EAPI PhongMaterial& SetDiffuse (Texture* diffuse);
    __EAPI PhongMaterial& SetNormal (Texture* normal);
    __EAPI PhongMaterial& SetSpecular (Texture* specular);
    __EAPI PhongMaterial& SetEmissive (Texture* emissive);

    __EAPI PhongMaterial& SetDiffuse (const Vector3f& diffuse);
    __EAPI PhongMaterial& SetSpecular (const Vector3f& specular);
    __EAPI PhongMaterial& SetSpecularShininess (const float specularShininess);
    __EAPI PhongMaterial& SetEmissive (const Vector3f& emissive);


    // getters
    __EAPI Texture* GetDiffuse ();
    __EAPI Texture* GetNormal ();
    __EAPI Texture* GetSpecular ();
    __EAPI float GetSpecularShininess ();
    __EAPI Texture* GetEmissive ();

protected:

    // static textures
    Texture diffuse;
    Texture normal;
    Texture specular;
    Texture emissive;

};

#endif