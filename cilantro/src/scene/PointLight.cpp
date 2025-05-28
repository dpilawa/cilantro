#include "cilantroengine.h"
#include "scene/GameScene.h"
#include "scene/PointLight.h"
#include "graphics/Renderer.h"
#include "system/Game.h"

namespace cilantro
{

PointLight::PointLight (std::shared_ptr<GameScene> gameScene) : Light (gameScene)
{
    m_attenuationConst = 1.0f;
    m_attenuationLinear = 0.0f;
    m_attenuationQuadratic = 0.0f;
}

PointLight::~PointLight ()
{
}

std::shared_ptr<PointLight> PointLight::SetConstantAttenuationFactor (const float attenuation)
{
    m_attenuationConst = attenuation;
    GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (this->GetHandle ()));
    return std::dynamic_pointer_cast<PointLight> (shared_from_this ());
}

std::shared_ptr<PointLight> PointLight::SetLinearAttenuationFactor (const float attenuation)
{
    m_attenuationLinear = attenuation;
    GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (this->GetHandle ()));
    return std::dynamic_pointer_cast<PointLight> (shared_from_this ());
}

std::shared_ptr<PointLight> PointLight::SetQuadraticAttenuationFactor (const float attenuation)
{
    m_attenuationQuadratic = attenuation;
    GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (this->GetHandle ()));
    return std::dynamic_pointer_cast<PointLight> (shared_from_this ());
}

float PointLight::GetConstantAttenuationFactor () const
{
    return m_attenuationConst;
}

float PointLight::GetLinearAttenuationFactor () const
{
    return m_attenuationLinear;
}

float PointLight::GetQuadraticAttenuationFactor () const
{
    return m_attenuationQuadratic;
}

float PointLight::GetBoundingSphereRadius (float threshold) const
{
    float c = m_attenuationConst - 1.0f / threshold;
    float d = m_attenuationLinear * m_attenuationLinear - 4.0f * m_attenuationQuadratic * c;

    if (d < 0.0f)
    {
        return 0.0f; // no valid radius
    }
    else if (d == 0.0f)
    {
        return -m_attenuationLinear / (2.0f * m_attenuationQuadratic);
    }
    else
    {
        float r1 = (-m_attenuationLinear + std::sqrt (d)) / (2.0f * m_attenuationQuadratic);
        float r2 = (-m_attenuationLinear - std::sqrt (d)) / (2.0f * m_attenuationQuadratic);
        return std::max (r1, r2); // return the larger radius
    }
}

void PointLight::OnUpdate (IRenderer& renderer)
{
    Light::OnUpdate (renderer);
    renderer.Update (std::dynamic_pointer_cast<PointLight> (shared_from_this ()));
}

} // namespace cilantro

