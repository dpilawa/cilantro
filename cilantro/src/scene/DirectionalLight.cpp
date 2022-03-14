#include "cilantroengine.h"
#include "scene/DirectionalLight.h"
#include "graphics/Renderer.h"
#include "system/CallbackProvider.h"

DirectionalLight::DirectionalLight (CGameScene* gameScene) : Light (gameScene)
{
}

DirectionalLight::~DirectionalLight ()
{
}

void DirectionalLight::OnUpdate (IRenderer& renderer)
{
    Light::OnUpdate (renderer);
    renderer.Update (*this);
}