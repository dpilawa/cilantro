#include "cilantroengine.h"
#include "scene/GameScene.h"
#include "scene/SpotLight.h"
#include "graphics/Renderer.h"
#include "system/Game.h"

namespace cilantro {

SpotLight::SpotLight (std::shared_ptr<GameScene> gameScene) : PointLight (gameScene)
{
    m_innerCutoff = 45.0f;
    m_outerCutoff = m_innerCutoff;
}

SpotLight::~SpotLight ()
{
}

SpotLight& SpotLight::SetInnerCutoff (const float cutoff)
{
    m_innerCutoff = cutoff;
    GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (this->GetHandle ()));

    return *this;
}

SpotLight& SpotLight::SetOuterCutoff (const float cutOff)
{
    m_outerCutoff = cutOff;
    GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (this->GetHandle ()));

    return *this;
}

float SpotLight::GetInnerCutoff () const
{
    return m_innerCutoff;
}

float SpotLight::GetOuterCutoff () const 
{
    return m_outerCutoff;
}

void SpotLight::OnUpdate (IRenderer& renderer)
{
    Light::OnUpdate (renderer);
    renderer.Update (std::dynamic_pointer_cast<SpotLight> (this->GetPointer ()));
}

} // namespace cilantro