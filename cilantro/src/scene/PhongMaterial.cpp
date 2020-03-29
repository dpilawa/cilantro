#include "cilantroengine.h"
#include "scene/PhongMaterial.h"
#include "math/Vector3f.h"

PhongMaterial::PhongMaterial ()
{
    SetShaderProgram ("phong_shader");
    SetColor (Vector3f (1.0f, 1.0f, 1.0f));
    SetEmissiveColor (Vector3f (0.0f, 0.0f, 0.0f));
    SetSpecularShininess (32.0f);
}

PhongMaterial::~PhongMaterial ()
{
}

PhongMaterial& PhongMaterial::SetAmbientColor (const Vector3f& color)
{
    ambientColor = color;
    properties["fAmbientColor"] = {color[0], color[1], color[2]};

    return *this;
}

PhongMaterial& PhongMaterial::SetDiffuseColor (const Vector3f& color)
{
    diffuseColor = color;
    properties["fDiffuseColor"] = {color[0], color[1], color[2]};

    return *this;
}

PhongMaterial& PhongMaterial::SetSpecularColor (const Vector3f & color)
{
    specularColor = color;
    properties["fSpecularColor"] = {color[0], color[1], color[2]};

    return *this;
}

PhongMaterial& PhongMaterial::SetSpecularShininess (const float shininess)
{
    specularShininess = shininess;
    properties["fSpecularShininess"] = {shininess};

    return *this;
}

PhongMaterial& PhongMaterial::SetEmissiveColor (const Vector3f & color)
{
    emissiveColor = color;
    properties["fEmissiveColor"] = {color[0], color[1], color[2]};

    return *this;
}

PhongMaterial& PhongMaterial::SetColor (const Vector3f & color)
{
    SetAmbientColor (color);
    SetDiffuseColor (color);
    SetSpecularColor (color);

    return *this;
}

Vector3f PhongMaterial::GetAmbientColor () const
{
    return ambientColor;
}

Vector3f PhongMaterial::GetDiffuseColor () const
{
    return diffuseColor;
}

Vector3f PhongMaterial::GetSpecularColor () const
{
    return specularColor;
}

float PhongMaterial::GetSpecularShininess () const
{
    return specularShininess;
}

Vector3f PhongMaterial::GetEmissiveColor () const
{
    return emissiveColor;
}

