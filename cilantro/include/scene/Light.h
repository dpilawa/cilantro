#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "math/Vector3f.h"

struct IRenderer;

namespace cilantro {

class __CEAPI Light : public GameObject
{
public:
    __EAPI Light (std::shared_ptr<GameScene> gameScene);
    __EAPI virtual ~Light ();

    // enable or disable light
    __EAPI std::shared_ptr<Light> SetEnabled (bool value);
    __EAPI bool IsEnabled () const;

    // set color of the light
    __EAPI std::shared_ptr<Light> SetColor (const Vector3f color);

    // get color of the light
    __EAPI Vector3f GetColor () const;

    // invoked by game loop on update	
    __EAPI void OnUpdate (IRenderer& renderStage);

private:

    bool m_isEnabled;
    Vector3f m_lightColor;
};

} // namespace cilantro

#endif

