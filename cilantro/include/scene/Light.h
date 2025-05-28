#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "math/Vector3f.h"
#include <array>

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

    // calculate tight light view projection for given camera frustum and scene AABB (for shadow mapping)
    __EAPI Matrix4f GenLightViewProjectionMatrix (const std::array<Vector3f, 8>& frustumVertices, const AABB& sceneAABB, bool isOrtho = true, float perspectiveFov = 45.0f, float limitFar = 100.0f);

private:

    bool m_isEnabled;
    Vector3f m_lightColor;
};

} // namespace cilantro

#endif

