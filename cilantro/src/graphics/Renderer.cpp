#include "cilantroengine.h"
#include "graphics/Renderer.h"
#include "graphics/RenderTarget.h"
#include "scene/GameScene.h"
#include "scene/MeshObject.h"
#include "system/Timer.h"

Renderer::Renderer (unsigned int width, unsigned int height)
{
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

ResourceManager<ShaderProgram>& Renderer::GetShaderProgramManager ()
{
    return shaderPrograms;
}

