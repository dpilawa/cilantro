#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <string>
#include "Vector3f.h"

class Material
{
public:
	Material ();
	~Material ();

	// setters
	Material& SetShaderModelName (std::string name);
	Material& SetAmbientColor (const Vector3f& color);
	Material& SetDiffuseColor (const Vector3f& color);
	Material& SetSpecularColor (const Vector3f& color);
	Material& SetSpecularShininess (const float shininess);
	Material& SetColor (const Vector3f& color);

	// getters
	std::string GetShaderModelName () const;
	Vector3f GetAmbientColor () const;
	Vector3f GetDiffuseColor () const;
	Vector3f GetSpecularColor () const;
	float GetSpecularShininess () const;

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

};

#endif