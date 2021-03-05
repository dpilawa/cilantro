#include "cilantroengine.h"
#include "graphics/Renderer.h"
#include "graphics/RenderTarget.h"
#include "scene/GameScene.h"
#include "scene/MeshObject.h"
#include "system/Timer.h"

Renderer::Renderer (unsigned int width, unsigned int height)
{
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

ResourceManager<Resource>& Renderer::GetGameResourceManager ()
{
    return game->GetResourceManager ();
}

ResourceManager<Postprocess>& Renderer::GetPostprocessManager ()
{
    return postprocesses;
}

ResourceManager<ShaderProgram>& Renderer::GetShaderProgramManager ()
{
    return shaderPrograms;
}

