#include "cilantroengine.h"
#include "scene/DirectionalLight.h"
#include "scene/Light.h"
#include "math/Vector3f.h"
#include "graphics/Renderer.h"
#include "util/CallbackProvider.h"

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