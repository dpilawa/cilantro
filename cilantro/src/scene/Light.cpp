#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "scene/Light.h"
#include "system/Game.h"

Light::Light (CGameScene* gameScene) : GameObject (gameScene)
{
    isEnabled = false;
    
    lightColor = Vector3f (1.0f, 1.0f, 1.0f);
}

Light::~Light ()
{
}

void Light::SetEnabled (bool value)
{
    isEnabled = value;
    CGame::GetMessageBus ().Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (this->GetHandle ()));
}

bool Light::IsEnabled () const
{
    return isEnabled;
}

Light& Light::SetColor (Vector3f color)
{
    lightColor = color;
    CGame::GetMessageBus ().Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (this->GetHandle ()));
    return *this;
}

Vector3f Light::GetColor () const
{
    if (isEnabled)
    {
        return lightColor;
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

