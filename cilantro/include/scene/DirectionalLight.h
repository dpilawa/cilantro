#ifndef _DIRECTIONALLIGHT_H_
#define _DIRECTIONALLIGHT_H_

#include "cilantroengine.h"
#include "scene/Light.h"

struct IRenderer;

namespace cilantro {

class __CEAPI DirectionalLight : public Light
{
public:
    __EAPI DirectionalLight (std::shared_ptr<GameScene> gameScene);
    __EAPI virtual ~DirectionalLight ();

    // invoked by game loop on update	
    __EAPI void OnUpdate (IRenderer& renderer);

private:

};

} // namespace cilantro

#endif