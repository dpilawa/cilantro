#ifndef _DIRECTIONALLIGHT_H_
#define _DIRECTIONALLIGHT_H_

#include "cilantroengine.h"
#include "scene/Light.h"
#include "math/Vector3f.h"

class GeometryRenderStage;

class DirectionalLight : public Light
{
public:
    __EAPI DirectionalLight (GameScene* gameScene);
    __EAPI virtual ~DirectionalLight ();

    // invoked by game loop on update	
    void OnUpdate (GeometryRenderStage& renderStage);

private:

};

#endif