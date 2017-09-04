#include "Renderer.h"

Renderer::Renderer ()
{
	frameCount = 0;
	renderedScene = nullptr;
}

Renderer::~Renderer ()
{
}

void Renderer::Initialize (GameScene * scene)
{
	renderedScene = scene;
}

void Renderer::RenderFrame ()
{
	frameCount++;
}

long long Renderer::GetFrameCount () const
{
	return frameCount;
}

