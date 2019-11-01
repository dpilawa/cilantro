#ifndef _POINTLIGHT_H_
#define _POINTLIGHT_H_

#include "cilantroengine.h"
#include "scene/Light.h"
#include "math/Vector3f.h"

class PointLight : public Light
{
public:
	__EAPI PointLight ();
	__EAPI virtual ~PointLight ();

	// set attenuation factors
	__EAPI PointLight& SetConstantAttenuationFactor (const float attenuation);
	__EAPI PointLight& SetLinearAttenuationFactor (const float attenuation);
	__EAPI PointLight& SetQuadraticAttenuationFactor (const float attenuation);

	// getters
	__EAPI float GetConstantAttenuationFactor () const;
	__EAPI float GetLinearAttenuationFactor () const;
	__EAPI float GetQuadraticAttenuationFactor () const;

	// invoked by game loop on update
    void OnUpdate (Renderer &renderer);

private:

	float attenuationConst;
	float attenuationLinear;
	float attenuationQuadratic;
};

#endif