#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "math/Vector3f.h"

struct IRenderer;

namespace cilantro {

class Light : public GameObject
{
public:
    Light (GameScene* gameScene);
    virtual ~Light ();

    // enable or disable light
    __EAPI void SetEnabled (bool value);
    __EAPI bool IsEnabled () const;

    // set color of the light
    __EAPI Light& SetColor (const Vector3f color);

    // get color of the light
    __EAPI Vector3f GetColor () const;


    // invoked by game loop on update	
    void OnUpdate (IRenderer& renderStage);

private:

    bool isEnabled;
    Vector3f lightColor;
};

} // namespace cilantro

#endif

