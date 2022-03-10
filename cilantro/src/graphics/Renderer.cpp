#include "graphics/Renderer.h"
#include "graphics/RenderStage.h"
#include "graphics/GLDeferredGeometryRenderStage.h"
#include "graphics/GLForwardGeometryRenderStage.h"
#include "graphics/Framebuffer.h"
#include "scene/GameScene.h"
#include "system/Timer.h"
#include "system/LogMessage.h"
#include <cmath>

CRenderer::CRenderer (GameScene* gameScene, unsigned int width, unsigned int height, bool isDeferred)
    : m_GameScene (gameScene)
    , m_IsDeferred (isDeferred)
    , m_Width (width)
    , m_Height (height)
{
    m_TotalRenderedFrames = 0L;
    m_TotalRenderTime = 0.0f;
    m_TotalFrameRenderTime = 0.0f;

    m_LightingShaderStagesCount = 0;
}

void CRenderer::Initialize ()
{
    InitializeRenderStages ();
}

void CRenderer::Deinitialize ()
{
    DeinitializeRenderStages ();

    LogMessage (MSG_LOCATION) << "Rendered" << m_TotalRenderedFrames << "frames in" << m_TotalRenderTime << "seconds; avg FPS =" << std::round (m_TotalRenderedFrames / m_TotalFrameRenderTime) << "; real FPS = " << std::round (m_TotalRenderedFrames / m_TotalRenderTime);
}

unsigned int CRenderer::GetWidth () const
{
    return this->m_Width;
}

unsigned int CRenderer::GetHeight () const
{
    return this->m_Height;
}

IRenderer& CRenderer::SetResolution (unsigned int width, unsigned int height)
{
    Framebuffer* fb;
    this->m_Width = width;
    this->m_Height = height;

    for (auto& stage : m_RenderStageManager)
    {
        fb = stage->GetFramebuffer ();
        if (fb != nullptr)
        {
            fb->SetFramebufferResolution (width, height);
        }
    }

    return *this;
}

GameScene* CRenderer::GetGameScene ()
{
    return m_GameScene;
}

TShaderProgramManager& CRenderer::GetShaderProgramManager ()
{
    return m_ShaderProgramManager;
}

TRenderStageManager& CRenderer::GetRenderStageManager ()
{
    return m_RenderStageManager;
}

TRenderPipeline& CRenderer::GetRenderPipeline ()
{
    return m_RenderPipeline;
}

IRenderer& CRenderer::RotateRenderPipelineLeft ()
{
    std::rotate (m_RenderPipeline.begin (), m_RenderPipeline.begin () + 1, m_RenderPipeline.end ());

    return *this;
}

IRenderer& CRenderer::RotateRenderPipelineRight ()
{
    std::rotate (m_RenderPipeline.rbegin (), m_RenderPipeline.rbegin () + 1, m_RenderPipeline.rend ());

    return *this;
}

Framebuffer* CRenderer::GetPipelineFramebuffer (EPipelineLink link)
{
    if ((m_CurrentRenderStage == 0 && link == EPipelineLink::LINK_PREVIOUS) || (m_RenderPipeline.size () < 2 && link == EPipelineLink::LINK_SECOND))
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Pipeline index out of bounds";
    }

    if (link == EPipelineLink::LINK_FIRST)
    {
        return GetRenderStageManager ().GetByHandle<RenderStage> (m_RenderPipeline.front ()).GetFramebuffer ();
    }
    else if (link == EPipelineLink::LINK_SECOND)
    {
        return GetRenderStageManager ().GetByHandle<RenderStage> (m_RenderPipeline[1]).GetFramebuffer ();
    }
    else if (link == EPipelineLink::LINK_PREVIOUS)
    {
        return GetRenderStageManager ().GetByHandle<RenderStage> (m_RenderPipeline[m_CurrentRenderStage - 1]).GetFramebuffer ();
    }
    else if (link == EPipelineLink::LINK_LAST)
    {
        return GetRenderStageManager ().GetByHandle<RenderStage> (m_RenderPipeline.back ()).GetFramebuffer ();
    }
    else /* LINK_CURRENT */
    {
        return GetRenderStageManager ().GetByHandle<RenderStage> (m_RenderPipeline[m_CurrentRenderStage]).GetFramebuffer ();
    }
}

void CRenderer::RenderFrame ()
{
    m_CurrentRenderStage = 0;

    // reset global rendering timer
    if (m_TotalRenderTime == 0L)
    {
        m_GameScene->GetTimer ()->ResetSplitTime ();
    }

    // run post-processing
    for (handle_t stageHandle : m_RenderPipeline)
    {
        m_RenderStageManager.GetByHandle<RenderStage> (stageHandle).OnFrame ();
        m_CurrentRenderStage++;
    }

    // update game clocks (Tock)
    m_GameScene->GetTimer ()->Tock ();

    // update frame counters
    m_TotalRenderedFrames++;
    m_TotalRenderTime = m_GameScene->GetTimer ()->GetTimeSinceSplitTime ();
    m_TotalFrameRenderTime += m_GameScene->GetTimer ()->GetFrameRenderTime ();
}

void CRenderer::InitializeRenderStages ()
{
    if (m_IsDeferred == true)
    {
        // geometry stage
        RenderStage& baseDeferred = this->AddRenderStage<GLDeferredGeometryRenderStage> ("base");
        baseDeferred.SetDepthTestEnabled (true);
        baseDeferred.SetStencilTestEnabled (true);
        baseDeferred.SetClearColorOnFrameEnabled (true);
        baseDeferred.SetClearDepthOnFrameEnabled (true);
        baseDeferred.SetClearStencilOnFrameEnabled (true);
        baseDeferred.Initialize ();
        
        // lighting stages (per material shader)
        for (auto&& material : m_GameScene->GetMaterialManager ())
        {
            this->Update (*material);
        }
    }
    else
    {
        RenderStage& baseForward = this->AddRenderStage<GLForwardGeometryRenderStage> ("base");
        baseForward.Initialize ();
    }
}

void CRenderer::DeinitializeRenderStages ()
{
    for (auto&& stage : m_RenderStageManager)
    {
        stage->Deinitialize ();
    }
}