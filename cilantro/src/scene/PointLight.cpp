#include "cilantroengine.h"
#include "scene/PointLight.h"
#include "graphics/Renderer.h"
#include "system/Game.h"

PointLight::PointLight (CGameScene* gameScene) : Light (gameScene)
{
    attenuationConst = 1.0f;
    attenuationLinear = 0.0f;
    attenuationQuadratic = 0.0f;
}

PointLight::~PointLight ()
{
}

PointLight& PointLight::SetConstantAttenuationFactor (const float attenuation)
{
    attenuationConst = attenuation;
    CGame::GetMessageBus ().Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (this->GetHandle ()));
    return *this;
}

PointLight& PointLight::SetLinearAttenuationFactor (const float attenuation)
{
    attenuationLinear = attenuation;
    CGame::GetMessageBus ().Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (this->GetHandle ()));
    return *this;
}

PointLight& PointLight::SetQuadraticAttenuationFactor (const float attenuation)
{
    attenuationQuadratic = attenuation;
    CGame::GetMessageBus ().Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (this->GetHandle ()));
    return *this;
}

float PointLight::GetConstantAttenuationFactor () const
{
    return attenuationConst;
}

float PointLight::GetLinearAttenuationFactor () const
{
    return attenuationLinear;
}

float PointLight::GetQuadraticAttenuationFactor () const
{
    return attenuationQuadratic;
}

void PointLight::OnUpdate (IRenderer& renderer)
{
    Light::OnUpdate (renderer);
    renderer.Update (*this);
}
