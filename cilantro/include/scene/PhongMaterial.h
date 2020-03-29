#ifndef _PHONGMATERIAL_H_
#define _PHONGMATERIAL_H_

#include "cilantroengine.h"
#include "scene/Material.h"
#include "math/Vector3f.h"

class PhongMaterial : public Material
{
public:
    __EAPI PhongMaterial ();
    __EAPI virtual ~PhongMaterial ();

    // setters
    __EAPI PhongMaterial& SetAmbientColor (const Vector3f& color);
    __EAPI PhongMaterial& SetDiffuseColor (const Vector3f& color);
    __EAPI PhongMaterial& SetSpecularColor (const Vector3f& color);
    __EAPI PhongMaterial& SetSpecularShininess (const float shininess);
    __EAPI PhongMaterial& SetEmissiveColor (const Vector3f& color);
    __EAPI PhongMaterial& SetColor (const Vector3f& color);

    // getters
    __EAPI Vector3f GetAmbientColor () const;
    __EAPI Vector3f GetDiffuseColor () const;
    __EAPI Vector3f GetSpecularColor () const;
    __EAPI float GetSpecularShininess () const;
    __EAPI Vector3f GetEmissiveColor () const;

private:

    // ambient color
    Vector3f ambientColor;

    // diffuse color
    Vector3f diffuseColor;

    // specular color
    Vector3f specularColor;
    float specularShininess;

    // emissive color
    Vector3f emissiveColor;

};

#endif