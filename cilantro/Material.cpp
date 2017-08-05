#include "Material.h"

Material::Material ()
{
	shaderModelName = "default_shader";
	diffuseColor = Vector3f (1.0f, 1.0f, 1.0f);
}

Material::~Material ()
{
}

Material & Material::SetShaderModelName (std::string name)
{
	shaderModelName = name;

	return *this;
}

Material & Material::SetDiffuseColor (const Vector3f& color)
{
	diffuseColor = color;

	return *this;
}

std::string Material::GetShaderModelName () const
{
	return shaderModelName;
}

Vector3f Material::GetDiffuseColor () const
{
	return diffuseColor;
}
