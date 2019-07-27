#ifndef _DIRECTIONALLIGHT_H_
#define _DIRECTIONALLIGHT_H_

#include "cilantroengine.h"
#include "Light.h"
#include "Vector3f.h"

class DirectionalLight : public Light
{
public:
	__EAPI DirectionalLight ();
	__EAPI ~DirectionalLight ();

    // setters
    __EAPI DirectionalLight& SetDirection (const Vector3f& newDirection);

    // getters
    __EAPI Vector3f GetDirection () const;

protected:

	// callback on light update to notify rendered
	void InvokeOnUpdateCallbacks ();

private:

	Vector3f direction;
};

#endif