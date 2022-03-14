#ifndef _DIRECTIONALLIGHT_H_
#define _DIRECTIONALLIGHT_H_

#include "cilantroengine.h"
#include "scene/Light.h"

struct IRenderer;

class DirectionalLight : public Light
{
public:
    __EAPI DirectionalLight (CGameScene* gameScene);
    __EAPI virtual ~DirectionalLight ();

    // invoked by game loop on update	
    void OnUpdate (IRenderer& renderer);

private:

};

#endif