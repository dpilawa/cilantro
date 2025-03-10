#include "graphics/Renderer.h"
#include "graphics/IRenderStage.h"
#include "graphics/ShadowMapRenderStage.h"
#include "graphics/DeferredGeometryRenderStage.h"
#include "graphics/ForwardGeometryRenderStage.h"
#include "graphics/IFramebuffer.h"
#include "scene/GameScene.h"
#include "system/Timer.h"
#include "system/LogMessage.h"
#include <cmath>

namespace cilantro {

Renderer::Renderer (GameScene* gameScene, unsigned int width, unsigned int height, bool shadowMappingEnabled, bool deferredRenderingEnabled)
    : m_gameScene (gameScene)
    , m_isDeferredRendering (deferredRenderingEnabled)
    , m_isShadowMapping (shadowMappingEnabled)
    , m_width (width)
    , m_height (height)
{
    m_totalRenderedFrames = 0L;
    m_totalRenderTime = 0.0f;
    m_totalFrameRenderTime = 0.0f;

    m_lightingShaderStagesCount = 0;
}

void Renderer::Initialize ()
{
    InitializeRenderStages ();
}

void Renderer::Deinitialize ()
{
    DeinitializeRenderStages ();

    LogMessage (MSG_LOCATION) << "Rendered" << m_totalRenderedFrames << "frames in" << m_totalRenderTime << "seconds; avg FPS =" << std::round (m_totalRenderedFrames / m_totalFrameRenderTime) << "; real FPS = " << std::round (m_totalRenderedFrames / m_totalRenderTime);
}

unsigned int Renderer::GetWidth () const
{
    return this->m_width;
}

unsigned int Renderer::GetHeight () const
{
    return this->m_height;
}

IRenderer& Renderer::SetResolution (unsigned int width, unsigned int height)
{
    IFramebuffer* fb;
    this->m_width = width;
    this->m_height = height;

    for (auto& stage : m_renderStageManager)
    {
        fb = stage->GetFramebuffer ();
        if (fb != nullptr)
        {
            fb->SetFramebufferResolution (width, height);
        }
    }

    return *this;
}

GameScene* Renderer::GetGameScene ()
{
    return m_gameScene;
}

TShaderProgramManager& Renderer::GetShaderProgramManager ()
{
    return m_shaderProgramManager;
}

TRenderStageManager& Renderer::GetRenderStageManager ()
{
    return m_renderStageManager;
}

IRenderStage* Renderer::GetCurrentRenderStage ()
{
    return m_currentRenderStage;
}

TRenderPipeline& Renderer::GetRenderPipeline ()
{
    return m_renderPipeline;
}

IRenderer& Renderer::RotateRenderPipelineLeft ()
{
    std::rotate (m_renderPipeline.begin (), m_renderPipeline.begin () + 1, m_renderPipeline.end ());

    return *this;
}

IRenderer& Renderer::RotateRenderPipelineRight ()
{
    std::rotate (m_renderPipeline.rbegin (), m_renderPipeline.rbegin () + 1, m_renderPipeline.rend ());

    return *this;
}

IFramebuffer* Renderer::GetPipelineFramebuffer (EPipelineLink link)
{
    if ((m_currentRenderStageIdx == 0 && link == EPipelineLink::LINK_PREVIOUS) || 
        (m_currentRenderStageIdx < 2 && link == EPipelineLink::LINK_PREVIOUS_MINUS_1) ||
        (m_renderPipeline.size () < 2 && link == EPipelineLink::LINK_SECOND) || 
        (m_renderPipeline.size () < 3 && link == EPipelineLink::LINK_THIRD))
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Pipeline index out of bounds";
    }

    if (link == EPipelineLink::LINK_FIRST)
    {
        return GetRenderStageManager ().GetByHandle<IRenderStage> (m_renderPipeline.front ()).GetFramebuffer ();
    }
    else if (link == EPipelineLink::LINK_SECOND)
    {
        return GetRenderStageManager ().GetByHandle<IRenderStage> (m_renderPipeline[1]).GetFramebuffer ();
    }
    else if (link == EPipelineLink::LINK_THIRD)
    {
        return GetRenderStageManager ().GetByHandle<IRenderStage> (m_renderPipeline[2]).GetFramebuffer ();
    }
    else if (link == EPipelineLink::LINK_PREVIOUS)
    {
        return GetRenderStageManager ().GetByHandle<IRenderStage> (m_renderPipeline[m_currentRenderStageIdx - 1]).GetFramebuffer ();
    }
    else if (link == EPipelineLink::LINK_PREVIOUS_MINUS_1)
    {
        return GetRenderStageManager ().GetByHandle<IRenderStage> (m_renderPipeline[m_currentRenderStageIdx - 2]).GetFramebuffer ();
    }
    else if (link == EPipelineLink::LINK_LAST)
    {
        return GetRenderStageManager ().GetByHandle<IRenderStage> (m_renderPipeline.back ()).GetFramebuffer ();
    }
    else /* LINK_CURRENT */
    {
        return GetRenderStageManager ().GetByHandle<IRenderStage> (m_renderPipeline[m_currentRenderStageIdx]).GetFramebuffer ();
    }
}

void Renderer::RenderFrame ()
{
    m_currentRenderStageIdx = 0;

    // reset global rendering timer
    if (m_totalRenderTime == 0L)
    {
        m_gameScene->GetTimer ()->ResetSplitTime ();
    }

    // run stages
    for (handle_t stageHandle : m_renderPipeline)
    {
        m_currentRenderStage = &m_renderStageManager.GetByHandle<IRenderStage> (stageHandle);
        m_currentRenderStage->OnFrame ();
        m_currentRenderStageIdx++;
    }

    // update game clocks (Tock)
    m_gameScene->GetTimer ()->Tock ();

    // update frame counters
    m_totalRenderedFrames++;
    m_totalRenderTime = m_gameScene->GetTimer ()->GetTimeSinceSplitTime ();
    m_totalFrameRenderTime += m_gameScene->GetTimer ()->GetFrameRenderTime ();
}

void Renderer::InitializeRenderStages ()
{
    if (m_isShadowMapping == true)
    {
        IRenderStage& shadow = this->Create<ShadowMapRenderStage> ("shadow_map");
        shadow.SetFaceCullingEnabled (true);
        shadow.SetFaceCullingMode (EFaceCullingFace::FACE_FRONT, EFaceCullingDirection::DIR_CCW);
        shadow.Initialize ();
    }

    if (m_isDeferredRendering == true)
    {
        // geometry stage
        IRenderStage& baseDeferred = this->Create<DeferredGeometryRenderStage> ("deferred_geometry");
        baseDeferred.SetDepthTestEnabled (true);
        baseDeferred.SetStencilTestEnabled (true);
        baseDeferred.SetClearColorOnFrameEnabled (true);
        baseDeferred.SetClearDepthOnFrameEnabled (true);
        baseDeferred.SetClearStencilOnFrameEnabled (true);
        baseDeferred.Initialize ();
        
        // lighting stages (per material shader)
        for (auto&& material : m_gameScene->GetMaterialManager ())
        {
            this->Update (*material);
        }
    }
    else
    {
        IRenderStage& baseForward = this->Create<ForwardGeometryRenderStage> ("forward");
        if (m_isShadowMapping == true)
        {
            baseForward.SetDepthArrayFramebufferLink (EPipelineLink::LINK_FIRST);
        }
        baseForward.Initialize ();
    }
}

void Renderer::DeinitializeRenderStages ()
{
    for (auto&& stage : m_renderStageManager)
    {
        stage->Deinitialize ();
    }
}

} // namespace cilantro