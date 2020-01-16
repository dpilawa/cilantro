#include "cilantroengine.h"
#include "graphics/Renderer.h"
#include "graphics/RenderTarget.h"
#include "scene/GameScene.h"
#include "scene/MeshObject.h"
#include "util/Timer.h"

Renderer::Renderer (GameLoop* gameLoop, unsigned int width, unsigned int height)
{
    this->gameLoop = gameLoop;

    this->width = width;
    this->height = height;
}

Renderer::~Renderer ()
{
}

void Renderer::RenderFrame ()
{
    // run post-processing
    postprocessStage = 0;
    for (auto&& postprocess : postprocesses)
    {
        postprocess->OnFrame ();
        postprocessStage++;
    }

    // update game clocks (Tock)
    Timer::Tock ();
}

void Renderer::SetResolution (unsigned int width, unsigned int height)
{
    this->width = width;
    this->height = height;
}

unsigned int Renderer::GetWidth () const
{
    return width;
}

unsigned int Renderer::GetHeight () const
{
    return height;
}

void Renderer::AddPostprocess (Postprocess* postprocess)
{
    postprocesses.push_back (postprocess);
}