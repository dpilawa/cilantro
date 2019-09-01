#include "cilantroengine.h"
#include "DirectionalLight.h"
#include "Light.h"
#include "Vector3f.h"
#include "Renderer.h"
#include "CallbackProvider.h"

DirectionalLight::DirectionalLight () : Light()
{
}

DirectionalLight::~DirectionalLight ()
{
}

void DirectionalLight::OnUpdate (Renderer& renderer)
{
	Light::OnUpdate (renderer);
	renderer.Update (*this);
}