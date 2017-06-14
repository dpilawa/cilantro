#include "Renderer.h"
#include "GameScene.h"

Renderer::Renderer (GameScene& scene) : renderedScene(scene)
{
	frameCount = 0;
}

Renderer::~Renderer ()
{
}

void Renderer::RenderFrame ()
{
	frameCount++;
}

long long Renderer::GetFrameCount () const
{
	return frameCount;
}

