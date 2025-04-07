#include "cilantroengine.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/Light.h"
#include "system/Game.h"

namespace cilantro {

Light::Light (std::shared_ptr<GameScene> gameScene) : GameObject (gameScene)
{
    m_isEnabled = false;
    
    m_lightColor = Vector3f (1.0f, 1.0f, 1.0f);
}

Light::~Light ()
{
}

std::shared_ptr<Light> Light::SetEnabled (bool value)
{
    m_isEnabled = value;
    GetGameScene ()->GetGame ()-> GetMessageBus ()->Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (this->GetHandle ()));
    return std::dynamic_pointer_cast<Light> (shared_from_this ());
}

bool Light::IsEnabled () const
{
    return m_isEnabled;
}

std::shared_ptr<Light> Light::SetColor (Vector3f color)
{
    m_lightColor = color;
    GetGameScene ()->GetGame ()-> GetMessageBus ()->Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (this->GetHandle ()));
    return std::dynamic_pointer_cast<Light> (shared_from_this ());
}

Vector3f Light::GetColor () const
{
    if (m_isEnabled)
    {
        return m_lightColor;
    }
    else
    {
        return Vector3f (0.0f, 0.0f, 0.0f);	
    }
}

void Light::OnUpdate (IRenderer& renderer)
{
    GameObject::OnUpdate (renderer);
}

} // namespace cilantro

