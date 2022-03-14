#include "cilantroengine.h"
#include "scene/SpotLight.h"
#include "graphics/Renderer.h"
#include "system/CallbackProvider.h"

SpotLight::SpotLight (CGameScene* gameScene) : PointLight (gameScene)
{
    innerCutoff = 45.0f;
    outerCutoff = innerCutoff;
}

SpotLight::~SpotLight ()
{
}

SpotLight& SpotLight::SetInnerCutoff (const float cutoff)
{
    innerCutoff = cutoff;
    InvokeOnUpdateCallbacks ();

    return *this;
}

SpotLight& SpotLight::SetOuterCutoff (const float cutOff)
{
    outerCutoff = cutOff;
    InvokeOnUpdateCallbacks ();

    return *this;
}

float SpotLight::GetInnerCutoff () const
{
    return innerCutoff;
}

float SpotLight::GetOuterCutoff () const 
{
    return outerCutoff;
}

void SpotLight::OnUpdate (IRenderer& renderer)
{
    Light::OnUpdate (renderer);
    renderer.Update (*this);
}