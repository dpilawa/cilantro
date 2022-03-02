#include "cilantroengine.h"
#include "scene/DirectionalLight.h"
#include "graphics/Renderer.h"
#include "system/CallbackProvider.h"

DirectionalLight::DirectionalLight (GameScene* gameScene) : Light (gameScene)
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