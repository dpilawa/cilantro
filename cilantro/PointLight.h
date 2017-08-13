#ifndef _POINTLIGHT_H_
#define _POINTLIGHT_H_

#include "Light.h"
#include "Vector3f.h"

class PointLight : public Light
{
public:
	PointLight ();
	~PointLight ();

	void Enable ();
	void Disable ();

	bool IsEnabled () const;

	void SetLightColor (Vector3f color);
	Vector3f GetLightColor () const;

private:
	Vector3f lightColor;
};

#endif