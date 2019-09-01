#include "cilantroengine.h"
#include "SpotLight.h"
#include "Vector3f.h"
#include "Renderer.h"
#include "Mathf.h"
#include "CallbackProvider.h"

SpotLight::SpotLight () : PointLight ()
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

void SpotLight::OnUpdate (Renderer& renderer)
{
	Light::OnUpdate (renderer);
	renderer.Update (*this);
}