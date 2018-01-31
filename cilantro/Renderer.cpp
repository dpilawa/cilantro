#include "Renderer.h"

Renderer::Renderer (GameScene& scene, RenderTarget& target)
{
	renderedScene = &scene;
	renderTarget = &target;
}

Renderer::~Renderer ()
{
}
