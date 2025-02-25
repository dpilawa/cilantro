#include "cilantroengine.h"
#include "scene/DirectionalLight.h"
#include "graphics/Renderer.h"

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