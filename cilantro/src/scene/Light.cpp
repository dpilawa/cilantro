#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "scene/Light.h"

Light::Light ()
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
    InvokeOnUpdateCallbacks ();
}

bool Light::IsEnabled () const
{
    return isEnabled;
}

Light& Light::SetColor (Vector3f color)
{
    lightColor = color;
    InvokeOnUpdateCallbacks ();
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

void Light::OnUpdate (Renderer& renderer)
{
    GameObject::OnUpdate (renderer);
}

void Light::InvokeOnUpdateCallbacks ()
{
    InvokeCallbacks ("OnUpdateLight", this->GetHandle (), 0);
}