#ifndef _POINTLIGHT_H_
#define _POINTLIGHT_H_

#include "cilantroengine.h"
#include "scene/Light.h"

struct IRenderer;

namespace cilantro {

class __CEAPI PointLight : public Light
{
public:
    __EAPI PointLight (std::shared_ptr<GameScene> gameScene);
    __EAPI virtual ~PointLight ();

    // set attenuation factors
    __EAPI std::shared_ptr<PointLight> SetConstantAttenuationFactor (const float attenuation);
    __EAPI std::shared_ptr<PointLight> SetLinearAttenuationFactor (const float attenuation);
    __EAPI std::shared_ptr<PointLight> SetQuadraticAttenuationFactor (const float attenuation);

    // set other parameters
    __EAPI std::shared_ptr<PointLight> SetEscapeRadius (const float radius);

    // getters
    __EAPI float GetConstantAttenuationFactor () const;
    __EAPI float GetLinearAttenuationFactor () const;
    __EAPI float GetQuadraticAttenuationFactor () const;
    __EAPI float GetEscapeRadius () const;

    // bounding sphere radius
    __EAPI float GetBoundingSphereRadius (float threshold) const;

    // invoked by game loop on update
    __EAPI void OnUpdate (IRenderer& renderer);

private:

    float m_attenuationConst;
    float m_attenuationLinear;
    float m_attenuationQuadratic;

    float m_escapeRadius; // escape radius of light (for shadow map generation where light is inside a mesh, e.g. lamp)
};

} // namespace cilantro

#endif