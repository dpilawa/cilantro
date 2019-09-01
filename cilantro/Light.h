#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "cilantroengine.h"
#include "GameObject.h"
#include "Vector3f.h"

class Renderer;

class Light : public GameObject
{
public:
	Light ();
	virtual ~Light ();

	// enable or disable light
	__EAPI void SetEnabled (bool value);
	__EAPI bool IsEnabled () const;

	// set color of the light
	__EAPI Light& SetColor (const Vector3f color);

	// set lighting powers
	__EAPI Light& SetAmbiencePower (const float ambience);
	__EAPI Light& SetSpecularPower (const float specular);

	__EAPI Vector3f GetColor () const;
	__EAPI float GetAmbiencePower () const;
	__EAPI float GetSpecularPower () const;

	// invoked by game loop on update	
    void OnUpdate (Renderer& renderer);

protected:

	// callback on light update to notify rendered
	virtual void InvokeOnUpdateCallbacks ();

private:

	bool isEnabled;

	Vector3f lightColor;
	float ambiencePower;
	float specularPower;
};

#endif

