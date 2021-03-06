#include "cilantroengine.h"
#include "graphics/Renderer.h"
#include "system/EngineContext.h"

Renderer::Renderer (unsigned int width, unsigned int height)
{
}

Renderer::~Renderer ()
{

}

void Renderer::Initialize ()
{
    
}

void Renderer::Deinitialize ()
{
    for (auto&& postprocess : postprocesses)
    {
        postprocess->Deinitialize ();
    }

    framebuffer->Deinitialize ();    
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
    EngineContext::GetTimer ().Tock ();
}

ResourceManager<Postprocess>& Renderer::GetPostprocessManager ()
{
    return postprocesses;
}

ResourceManager<ShaderProgram>& Renderer::GetShaderProgramManager ()
{
    return shaderPrograms;
}

