#include "cilantroengine.h"
#include "Renderer.h"
#include "RenderTarget.h"
#include "GameScene.h"
#include "MeshObject.h"
#include <string>

Renderer::Renderer (GameScene& scene, RenderTarget& target)
{
	renderedScene = &scene;
	renderTarget = &target;
}

Renderer::~Renderer ()
{
}
