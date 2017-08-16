#include "Material.h"

Material::Material ()
{
	shaderModelName = "default_shader";
	SetColor (Vector3f (1.0f, 1.0f, 1.0f));
	SetSpecularShininess (32.0f);
}

Material::~Material ()
{
}

Material & Material::SetShaderModelName (std::string name)
{
	shaderModelName = name;

	return *this;
}

Material & Material::SetAmbientColor (const Vector3f & color)
{
	ambientColor = color;

	return *this;
}

Material & Material::SetDiffuseColor (const Vector3f& color)
{
	diffuseColor = color;

	return *this;
}

Material & Material::SetSpecularColor (const Vector3f & color)
{
	specularColor = color;

	return *this;
}

Material & Material::SetSpecularShininess (const float shininess)
{
	specularShininess = shininess;

	return *this;
}

Material & Material::SetColor (const Vector3f & color)
{
	SetAmbientColor (color);
	SetDiffuseColor (color);
	SetSpecularColor (color);

	return *this;
}

std::string Material::GetShaderModelName () const
{
	return shaderModelName;
}

Vector3f Material::GetAmbientColor () const
{
	return ambientColor;
}

Vector3f Material::GetDiffuseColor () const
{
	return diffuseColor;
}

Vector3f Material::GetSpecularColor () const
{
	return specularColor;
}

float Material::GetSpecularShininess () const
{
	return specularShininess;
}
