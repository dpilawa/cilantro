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
	Material& SetDiffuseColor (const Vector3f& color);

	// getters
	std::string GetShaderModelName () const;
	Vector3f GetDiffuseColor () const;

private:

	// name of a shader program defined in renderer
	std::string shaderModelName;

	// diffuse color
	Vector3f diffuseColor;
};

#endif