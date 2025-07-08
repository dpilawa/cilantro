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

Renderer::Renderer (std::shared_ptr<GameScene> gameScene, unsigned int width, unsigned int height, bool shadowMappingEnabled, bool deferredRenderingEnabled)
    : m_gameScene (gameScene)
    , m_isDeferredRendering (deferredRenderingEnabled)
    , m_isShadowMapping (shadowMappingEnabled)
    , m_width (width)
    , m_height (height)
{
    m_totalRenderedFrames = 0L;
    m_totalDroppedFrames = 0L;
    m_totalRenderTime = 0.0f;
    m_totalFrameRenderTime = 0.0f;

    m_lightingShaderStagesCount = 0;

    m_renderStageManager = std::make_shared<TRenderStageManager> ();
    m_shaderProgramManager = std::make_shared<TShaderProgramManager> ();
}

Renderer::~Renderer ()
{
}

void Renderer::Initialize ()
{
    InitializeRenderStages ();
}

void Renderer::Deinitialize ()
{
    DeinitializeRenderStages ();

    LogMessage (MSG_LOCATION) << "Rendered" << m_totalRenderedFrames << "frames in" << m_totalRenderTime << "seconds; thoretical FPS =" << std::round (m_totalRenderedFrames / m_totalFrameRenderTime) << "; real FPS = " << std::round (m_totalRenderedFrames / m_totalRenderTime);
    LogMessage (MSG_LOCATION) << "Dropped frames:" << std::max ((long int)(m_totalRenderTime / (1.0f / CILANTRO_FPS)) - m_totalRenderedFrames, 0L);
}

unsigned int Renderer::GetWidth () const
{
    return this->m_width;
}

unsigned int Renderer::GetHeight () const
{
    return this->m_height;
}

std::shared_ptr<IRenderer> Renderer::SetResolution (unsigned int width, unsigned int height)
{
    this->m_width = width;
    this->m_height = height;

    for (auto& stage : m_renderStageManager)
    {
        auto fb = stage->GetFramebuffer ();
        if (fb != nullptr)
        {
            fb->SetFramebufferResolution (width, height);
        }
    }

    return std::dynamic_pointer_cast<IRenderer> (shared_from_this ());
}

std::shared_ptr<GameScene> Renderer::GetGameScene ()
{
    return m_gameScene.lock ();
}

std::shared_ptr<TShaderProgramManager> Renderer::GetShaderProgramManager ()
{
    return m_shaderProgramManager;
}

std::shared_ptr<TRenderStageManager> Renderer::GetRenderStageManager ()
{
    return m_renderStageManager;
}

std::shared_ptr<IRenderStage> Renderer::GetCurrentRenderStage ()
{
    return m_currentRenderStage;
}

TRenderPipeline& Renderer::GetRenderPipeline ()
{
    return m_renderPipeline;
}

std::shared_ptr<IRenderer> Renderer::RotateRenderPipelineLeft ()
{
    std::rotate (m_renderPipeline.begin (), m_renderPipeline.begin () + 1, m_renderPipeline.end ());

    return std::dynamic_pointer_cast<IRenderer> (shared_from_this ());
}

std::shared_ptr<IRenderer> Renderer::RotateRenderPipelineRight ()
{
    std::rotate (m_renderPipeline.rbegin (), m_renderPipeline.rbegin () + 1, m_renderPipeline.rend ());

    return std::dynamic_pointer_cast<IRenderer> (shared_from_this ());
}

std::shared_ptr<IFramebuffer> Renderer::GetPipelineFramebuffer (EPipelineLink link)
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
        return GetRenderStageManager ()->GetByHandle<IRenderStage> (m_renderPipeline.front ())->GetFramebuffer ();
    }
    else if (link == EPipelineLink::LINK_SECOND)
    {
        return GetRenderStageManager ()->GetByHandle<IRenderStage> (m_renderPipeline[1])->GetFramebuffer ();
    }
    else if (link == EPipelineLink::LINK_THIRD)
    {
        return GetRenderStageManager ()->GetByHandle<IRenderStage> (m_renderPipeline[2])->GetFramebuffer ();
    }
    else if (link == EPipelineLink::LINK_PREVIOUS)
    {
        return GetRenderStageManager ()->GetByHandle<IRenderStage> (m_renderPipeline[m_currentRenderStageIdx - 1])->GetFramebuffer ();
    }
    else if (link == EPipelineLink::LINK_PREVIOUS_MINUS_1)
    {
        return GetRenderStageManager ()->GetByHandle<IRenderStage> (m_renderPipeline[m_currentRenderStageIdx - 2])->GetFramebuffer ();
    }
    else if (link == EPipelineLink::LINK_LAST)
    {
        return GetRenderStageManager ()->GetByHandle<IRenderStage> (m_renderPipeline.back ())->GetFramebuffer ();
    }
    else /* LINK_CURRENT */
    {
        return GetRenderStageManager ()->GetByHandle<IRenderStage> (m_renderPipeline[m_currentRenderStageIdx])->GetFramebuffer ();
    }
}

void Renderer::RenderFrame ()
{
    m_currentRenderStageIdx = 0;

    // reset global rendering timer
    if (m_totalRenderTime == 0L)
    {
        GetGameScene ()->GetTimer ()->ResetSplitTime ();
    }

    // run stages
    for (handle_t stageHandle : m_renderPipeline)
    {
        m_currentRenderStage = m_renderStageManager->GetByHandle<IRenderStage> (stageHandle);
        m_currentRenderStage->OnFrame ();
        m_currentRenderStageIdx++;
    }

    // reset invalidated objects
    m_invalidatedObjects.clear ();

    // update frame counters
    m_totalRenderedFrames++;
    m_totalRenderTime = GetGameScene ()->GetTimer ()->GetTimeSinceSplitTime ();
    m_totalFrameRenderTime += GetGameScene ()->GetTimer ()->GetFrameRenderTime ();
}

AABB Renderer::CalculateAABB (std::shared_ptr<MeshObject> meshObject)
{
    AABB aabb;

    // calculate in CPU

    auto mesh = meshObject->GetMesh ();
    float* data = mesh->GetVerticesData ();
    Matrix4f worldTransform = meshObject->GetWorldTransformMatrix ();

    // apply bone transformations
    auto boneTransformations = meshObject->GetBoneTransformationsMatrixArray ();
    for (size_t v = 0; v < mesh->GetVertexCount (); v++)
    {
        Vector3f modelVertex (data[v * 3], data[v * 3 + 1], data[v * 3 + 2]);
        Vector4f worldVertex = worldTransform * Vector4f (modelVertex, 1.0f);
        Vector4f transformedVertex = Vector4f (0.0f, 0.0f, 0.0f, 0.0f);
        bool transformed = false;

        for (size_t i = 0; i < mesh->GetBoneInfluenceCounts ()[v]; i++)
        {
            size_t boneIndex = mesh->GetBoneIndicesData ()[v * CILANTRO_MAX_BONE_INFLUENCES + i];
            float boneWeight = mesh->GetBoneWeightsData ()[v * CILANTRO_MAX_BONE_INFLUENCES + i];
            
            transformedVertex += boneWeight * Matrix4f (boneTransformations + boneIndex * 16) * worldVertex;
            transformed = true;
        }

        transformed ? aabb.AddVertex (transformedVertex) : aabb.AddVertex (worldVertex);
    }

    return aabb;
}

bool Renderer::IsDeferredRendering () const
{
    return m_isDeferredRendering;
}

bool Renderer::IsShadowMapping () const
{
    return m_isShadowMapping;
}

void Renderer::InitializeRenderStages ()
{
    if (m_isShadowMapping == true)
    {
        auto shadow = this->Create<ShadowMapRenderStage> ("shadow_map");
        shadow->SetFaceCullingEnabled (true);
        shadow->SetFaceCullingMode (EFaceCullingFace::FACE_FRONT, EFaceCullingDirection::DIR_CCW);
        shadow->Initialize ();
    }

    if (m_isDeferredRendering == true)
    {
        // geometry stage
        auto baseDeferred = this->Create<DeferredGeometryRenderStage> ("deferred_geometry");
        baseDeferred->SetDepthTestEnabled (true);
        baseDeferred->SetStencilTestEnabled (true);
        baseDeferred->SetClearColorOnFrameEnabled (true);
        baseDeferred->SetClearDepthOnFrameEnabled (true);
        baseDeferred->SetClearStencilOnFrameEnabled (true);
        baseDeferred->Initialize ();
        
        // lighting stages (per material shader)
        for (auto&& material : GetGameScene ()->GetMaterialManager ())
        {
            this->Update (material);
        }
    }
    else
    {
        auto baseForward = this->Create<ForwardGeometryRenderStage> ("forward");
        if (m_isShadowMapping == true)
        {
            baseForward->SetDepthTextureArrayFramebufferLink (EPipelineLink::LINK_FIRST);
            baseForward->SetDepthCubeMapArrayFramebufferLink (EPipelineLink::LINK_FIRST);
        }
        baseForward->Initialize ();
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