#include "cilantroengine.h"
#include "scene/DirectionalLight.h"
#include "graphics/Renderer.h"

namespace cilantro {

DirectionalLight::DirectionalLight (std::shared_ptr<GameScene> gameScene) : Light (gameScene)
{
}

DirectionalLight::~DirectionalLight ()
{
}

void DirectionalLight::OnUpdate (IRenderer& renderer)
{
    Light::OnUpdate (renderer);
    renderer.Update (std::dynamic_pointer_cast<DirectionalLight> (shared_from_this ()));
}

} // namespace cilantro