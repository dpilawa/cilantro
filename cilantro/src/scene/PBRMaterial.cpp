#include "cilantroengine.h"
#include "scene/PBRMaterial.h"
#include "math/Vector3f.h"

PBRMaterial::PBRMaterial ()
{
    SetShaderProgram ("pbr_shader");
    SetAlbedo (Vector3f (1.0f, 1.0f, 1.0f));
    SetMetallic (0.0f);
    SetRoughness (1.0f);
    SetAO (1.0f);
}

PBRMaterial::~PBRMaterial ()
{
}

PBRMaterial& PBRMaterial::SetAlbedo (const Vector3f& albedo)
{
    this->albedo = albedo;
    properties["fAlbedo"] = {albedo[0], albedo[1], albedo[2]};

    return *this;
}

PBRMaterial& PBRMaterial::SetMetallic (const float metallic)
{
    this->metallic = metallic;
    properties["fMetallic"] = {metallic};

    return *this;
}

PBRMaterial& PBRMaterial::SetRoughness (const float roughness)
{
    this->roughhess = roughness;
    properties["fRoughness"] = {roughhess};

    return *this;
}

PBRMaterial& PBRMaterial::SetAO (const float ao)
{
    this->ao = ao;
    properties["fAO"] = {ao};

    return *this;
}

Vector3f PBRMaterial::GetAlbedo () const
{
    return albedo;
}

float PBRMaterial::GetMetallic () const
{
    return metallic;
}

float PBRMaterial::GetRoughness () const
{
    return roughhess;
}

float PBRMaterial::GetAO () const
{
    return ao;
}