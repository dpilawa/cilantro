#ifndef _DIRECTIONALLIGHT_H_
#define _DIRECTIONALLIGHT_H_

#include "cilantroengine.h"
#include "scene/Light.h"
#include "math/Vector3f.h"

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