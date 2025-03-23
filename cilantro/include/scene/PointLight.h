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
    __EAPI PointLight& SetConstantAttenuationFactor (const float attenuation);
    __EAPI PointLight& SetLinearAttenuationFactor (const float attenuation);
    __EAPI PointLight& SetQuadraticAttenuationFactor (const float attenuation);

    // getters
    __EAPI float GetConstantAttenuationFactor () const;
    __EAPI float GetLinearAttenuationFactor () const;
    __EAPI float GetQuadraticAttenuationFactor () const;

    // invoked by game loop on update
    __EAPI void OnUpdate (IRenderer& renderer);

private:

    float m_attenuationConst;
    float m_attenuationLinear;
    float m_attenuationQuadratic;
};

} // namespace cilantro

#endif