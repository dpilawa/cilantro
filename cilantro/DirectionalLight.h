#ifndef _DIRECTIONALLIGHT_H_
#define _DIRECTIONALLIGHT_H_

#include "cilantroengine.h"
#include "Light.h"
#include "Vector3f.h"

class DirectionalLight : public Light
{
public:
	__EAPI DirectionalLight ();
	__EAPI virtual ~DirectionalLight ();

	// invoked by game loop on update	
    void OnUpdate (Renderer& renderer);

private:

};

#endif