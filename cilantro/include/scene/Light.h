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
    __EAPI Light (GameScene* gameScene);
    __EAPI virtual ~Light ();

    // enable or disable light
    __EAPI Light& SetEnabled (bool value);
    __EAPI bool IsEnabled () const;

    // set color of the light
    __EAPI Light& SetColor (const Vector3f color);

    // get color of the light
    __EAPI Vector3f GetColor () const;

    // invoked by game loop on update	
    __EAPI void OnUpdate (IRenderer& renderStage);

private:

    bool isEnabled;
    Vector3f lightColor;
};

} // namespace cilantro

#endif

