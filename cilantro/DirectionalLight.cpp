#include "cilantroengine.h"
#include "DirectionalLight.h"
#include "Light.h"
#include "Vector3f.h"
#include "Renderer.h"
#include "CallbackProvider.h"

DirectionalLight::DirectionalLight () : Light()
{
	direction = Vector3f (0.0f, -1.0f, 0.0f);
}

DirectionalLight::~DirectionalLight ()
{
}

DirectionalLight& DirectionalLight::SetDirection (const Vector3f& newDirection)
{
    direction = newDirection;
    InvokeOnUpdateCallbacks ();

    return *this;
}

Vector3f DirectionalLight::GetDirection () const
{
    return direction;
}

void DirectionalLight::OnUpdate (Renderer& renderer)
{
	Light::OnUpdate (renderer);
	renderer.Update (*this);
}