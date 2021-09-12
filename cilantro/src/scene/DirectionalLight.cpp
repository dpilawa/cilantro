#include "cilantroengine.h"
#include "scene/DirectionalLight.h"
#include "scene/Light.h"
#include "math/Vector3f.h"
#include "graphics/GeometryRenderStage.h"
#include "system/CallbackProvider.h"

DirectionalLight::DirectionalLight () : Light()
{
}

DirectionalLight::~DirectionalLight ()
{
}

void DirectionalLight::OnUpdate (GeometryRenderStage& renderStage)
{
    Light::OnUpdate (renderStage);
    renderStage.Update (*this);
}