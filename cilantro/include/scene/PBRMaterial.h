#ifndef _PBRMATERIAL_H_
#define _PBRMATERIAL_H_

#include "cilantroengine.h"
#include "scene/Material.h"
#include "math/Vector3f.h"

class PBRMaterial : public Material
{
public:
    __EAPI PBRMaterial ();
    __EAPI virtual ~PBRMaterial ();

    // setters
    __EAPI PBRMaterial& SetAlbedo (const Vector3f& albedo);
    __EAPI PBRMaterial& SetMetallic (const float metallic);
    __EAPI PBRMaterial& SetRoughness (const float roughness);
    __EAPI PBRMaterial& SetAO (const float ao);


    // getters
    __EAPI Vector3f GetAlbedo () const;
    __EAPI float GetMetallic () const;
    __EAPI float GetRoughness () const;
    __EAPI float GetAO () const;

private:

    // albedo
    Vector3f albedo;

    // metallic
    float metallic;

    // rpughness
    float roughhess;

    // ambient occlusion
    float ao;

};

#endif