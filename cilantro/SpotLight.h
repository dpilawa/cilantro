#ifndef _SPOTLIGHT_H_
#define _SPOTLIGHT_H_

#include "cilantroengine.h"
#include "PointLight.h"
#include "Vector3f.h"

class SpotLight : public PointLight
{
public:
	__EAPI SpotLight ();
	__EAPI virtual ~SpotLight ();

	// set attenuation factors
	__EAPI SpotLight& SetInnerCutoff (const float cutoff);
	__EAPI SpotLight& SetOuterCutoff (const float cutOff);

	// getters
	__EAPI float GetInnerCutoff () const;
	__EAPI float GetOuterCutoff () const;

	// invoked by game loop on update
    void OnUpdate (Renderer &renderer);

private:

	float innerCutoff;
	float outerCutoff;
};

#endif