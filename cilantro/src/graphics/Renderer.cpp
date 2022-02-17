#include "graphics/Renderer.h"
#include "graphics/RenderStage.h"
#include "graphics/Framebuffer.h"
#include "scene/GameScene.h"
#include "system/Timer.h"
#include "system/LogMessage.h"
#include <cmath>

Renderer::Renderer (GameScene* gameScene, unsigned int width, unsigned int height)
{
    totalRenderFrames = 0L;
    totalRenderTime = 0.0f;
    totalFrameRenderTime = 0.0f;

    this->gameScene = gameScene;
    this->quadGeometryBuffer = nullptr;
    this->sceneGeometryBuffer = nullptr;

    this->width = width;
    this->height = height;
}

Renderer::~Renderer ()
{
}

void Renderer::Initialize ()
{
    for (auto&& stage : renderStages)
    {
        stage->Initialize ();
    }
}

void Renderer::Deinitialize ()
{
    for (auto&& stage : renderStages)
    {
        stage->Deinitialize ();
    }

    LogMessage (MSG_LOCATION) << "Rendered" << totalRenderFrames << "frames in" << totalRenderTime << "seconds; avg FPS =" << std::round (totalRenderFrames / totalFrameRenderTime) << "; real FPS = " << std::round (totalRenderFrames / totalRenderTime);
}

unsigned int Renderer::GetWidth () const
{
    return this->width;
}

unsigned int Renderer::GetHeight () const
{
    return this->height;
}

Renderer& Renderer::SetResolution (unsigned int width, unsigned int height)
{
    Framebuffer* fb;
    this->width = width;
    this->height = height;

    for (auto& stage : renderStages)
    {
        fb = stage->GetFramebuffer ();
        if (fb != nullptr)
        {
            fb->SetFramebufferResolution (width, height);
        }
    }

    return *this;
}

void Renderer::RenderFrame ()
{

    renderStage = 0;

    // reset global rendering timer
    if (totalRenderTime == 0L)
    {
        gameScene->GetTimer ()->ResetSplitTime ();
    }

    // run post-processing
    for (handle_t stageHandle : renderPipeline)
    {
        renderStages.GetByHandle<RenderStage> (stageHandle).OnFrame ();
        renderStage++;
    }

    // update game clocks (Tock)
    gameScene->GetTimer ()->Tock ();

    // update frame counters
    totalRenderFrames++;
    totalRenderTime = gameScene->GetTimer ()->GetTimeSinceSplitTime ();
    totalFrameRenderTime += gameScene->GetTimer ()->GetFrameRenderTime ();
}

GameScene* Renderer::GetGameScene ()
{
    return gameScene;
}

GeometryBuffer* Renderer::GetSceneGeometryBuffer () const
{
    return sceneGeometryBuffer;
}

GeometryBuffer* Renderer::GetQuadGeometryBuffer () const
{
    return quadGeometryBuffer;
}

Renderer& Renderer::RotateRenderPipelineLeft ()
{
    std::rotate (renderPipeline.begin (), renderPipeline.begin () + 1, renderPipeline.end ());

    return *this;
}

Renderer& Renderer::RotateRenderPipelineRight ()
{
    std::rotate (renderPipeline.rbegin (), renderPipeline.rbegin () + 1, renderPipeline.rend ());

    return *this;
}

std::vector<handle_t>& Renderer::GetRenderPipeline ()
{
    return renderPipeline;
}

Framebuffer* Renderer::GetPipelineFramebuffer (PipelineLink link)
{
    if ((renderStage == 0 && link == PipelineLink::LINK_PREVIOUS) || (renderPipeline.size () < 2 && link == PipelineLink::LINK_SECOND))
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Pipeline index out of bounds";
    }

    if (link == PipelineLink::LINK_FIRST)
    {
        return GetRenderStageManager ().GetByHandle<RenderStage> (renderPipeline.front ()).GetFramebuffer ();
    }
    else if (link == PipelineLink::LINK_SECOND)
    {
        return GetRenderStageManager ().GetByHandle<RenderStage> (renderPipeline[1]).GetFramebuffer ();
    }
    else if (link == PipelineLink::LINK_PREVIOUS)
    {
        return GetRenderStageManager ().GetByHandle<RenderStage> (renderPipeline[renderStage - 1]).GetFramebuffer ();
    }
    else if (link == PipelineLink::LINK_LAST)
    {
        return GetRenderStageManager ().GetByHandle<RenderStage> (renderPipeline.back ()).GetFramebuffer ();
    }
    else /* LINK_CURRENT */
    {
        return GetRenderStageManager ().GetByHandle<RenderStage> (renderPipeline[renderStage]).GetFramebuffer ();
    }
}

ResourceManager<RenderStage>& Renderer::GetRenderStageManager ()
{
    return renderStages;
}

ResourceManager<ShaderProgram>& Renderer::GetShaderProgramManager ()
{
    return shaderPrograms;
}

