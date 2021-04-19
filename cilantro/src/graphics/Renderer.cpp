#include "cilantroengine.h"
#include "graphics/Renderer.h"
#include "system/EngineContext.h"
#include "system/LogMessage.h"
#include <cmath>

Renderer::Renderer (unsigned int width, unsigned int height)
{
    totalRenderFrames = 0L;
    totalRenderTime = 0.0f;
    totalFrameRenderTime = 0.0f;


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

    LogMessage (MSG_LOCATION) << "Rendered" << totalRenderFrames << "frames in" << totalRenderTime << "seconds; avg FPS =" << std::round (totalRenderFrames / totalFrameRenderTime) << "; real FPS = " << std::round (totalRenderFrames / totalRenderTime);
}

void Renderer::RenderFrame ()
{
    // reset global rendering timer
    if (totalRenderTime == 0L)
    {
        EngineContext::GetTimer ().ResetSplitTime ();
    }

    // run post-processing
    postprocessStage = 0;
    for (auto&& postprocess : postprocesses)
    {
        postprocess->OnFrame ();
        postprocessStage++;
    }

    // update game clocks (Tock)
    EngineContext::GetTimer ().Tock ();

    // update frame counters
    totalRenderFrames++;
    totalRenderTime = EngineContext::GetTimer ().GetTimeSinceSplitTime ();
    totalFrameRenderTime += EngineContext::GetTimer ().GetFrameRenderTime ();
}

ResourceManager<Postprocess>& Renderer::GetPostprocessManager ()
{
    return postprocesses;
}

ResourceManager<ShaderProgram>& Renderer::GetShaderProgramManager ()
{
    return shaderPrograms;
}

