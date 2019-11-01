#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "cilantroengine.h"
#include "math/Vector3f.h"
#include <string>

class Material
{
public:
	__EAPI Material ();
	__EAPI ~Material ();

	// setters
	__EAPI Material& SetShaderModelName (std::string name);
	__EAPI Material& SetAmbientColor (const Vector3f& color);
	__EAPI Material& SetDiffuseColor (const Vector3f& color);
	__EAPI Material& SetSpecularColor (const Vector3f& color);
	__EAPI Material& SetSpecularShininess (const float shininess);
	__EAPI Material& SetEmissiveColor (const Vector3f& emissive);
	__EAPI Material& SetColor (const Vector3f& color);

	// getters
	__EAPI std::string GetShaderModelName () const;
	__EAPI Vector3f GetAmbientColor () const;
	__EAPI Vector3f GetDiffuseColor () const;
	__EAPI Vector3f GetSpecularColor () const;
	__EAPI float GetSpecularShininess () const;
	__EAPI Vector3f GetEmissiveColor () const;

private:

	// name of a shader program defined in renderer
	std::string shaderModelName;

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