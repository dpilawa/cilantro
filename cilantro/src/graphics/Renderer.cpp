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
    for (auto&& stage : renderStages)
    {
        stage->Deinitialize ();
    }

    framebuffer->Deinitialize ();    

    LogMessage (MSG_LOCATION) << "Rendered" << totalRenderFrames << "frames in" << totalRenderTime << "seconds; avg FPS =" << std::round (totalRenderFrames / totalFrameRenderTime) << "; real FPS = " << std::round (totalRenderFrames / totalRenderTime);
}

void Renderer::RenderFrame ()
{

    renderStage = 0;

    // reset global rendering timer
    if (totalRenderTime == 0L)
    {
        EngineContext::GetTimer ().ResetSplitTime ();
    }

    // run post-processing
    for (handle_t stageHandle : renderPipeline)
    {
        renderStage++;
        renderStages.GetByHandle<RenderStage> (stageHandle).OnFrame ();
    }

    // update game clocks (Tock)
    EngineContext::GetTimer ().Tock ();

    // update frame counters
    totalRenderFrames++;
    totalRenderTime = EngineContext::GetTimer ().GetTimeSinceSplitTime ();
    totalFrameRenderTime += EngineContext::GetTimer ().GetFrameRenderTime ();
}

ResourceManager<RenderStage>& Renderer::GetRenderStageManager ()
{
    return renderStages;
}

ResourceManager<ShaderProgram>& Renderer::GetShaderProgramManager ()
{
    return shaderPrograms;
}

