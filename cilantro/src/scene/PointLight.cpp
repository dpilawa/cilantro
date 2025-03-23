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

PointLight& PointLight::SetConstantAttenuationFactor (const float attenuation)
{
    m_attenuationConst = attenuation;
    GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (this->GetHandle ()));
    return *this;
}

PointLight& PointLight::SetLinearAttenuationFactor (const float attenuation)
{
    m_attenuationLinear = attenuation;
    GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (this->GetHandle ()));
    return *this;
}

PointLight& PointLight::SetQuadraticAttenuationFactor (const float attenuation)
{
    m_attenuationQuadratic = attenuation;
    GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (this->GetHandle ()));
    return *this;
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

void PointLight::OnUpdate (IRenderer& renderer)
{
    Light::OnUpdate (renderer);
    renderer.Update (std::dynamic_pointer_cast<PointLight> (this->GetPointer ()));
}

} // namespace cilantro

