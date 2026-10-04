#include "graphics/Renderer.h"
#include "graphics/IRenderStage.h"
#include "graphics/RenderStageNames.h"
#include "graphics/ShadowMapRenderStage.h"
#include "graphics/DeferredGeometryRenderStage.h"
#include "graphics/ForwardGeometryRenderStage.h"
#include "graphics/DeferredLightingRenderStage.h"
#include "graphics/IFramebuffer.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/MeshObject.h"
#include "scene/Light.h"
#include "scene/Material.h"
#include "scene/Camera.h"
#include "system/Game.h"
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
    m_lastLightingStageHandle = 0;

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

std::shared_ptr<IFramebuffer> Renderer::GetPipelineFramebuffer (const PipelineLink& link)
{
    if (link.IsNamed ())
    {
        if (!m_renderStageManager->HasName<IRenderStage> (link.GetStageName ()))
        {
            LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Pipeline link to unknown render stage" << link.GetStageName ();
        }

        return m_renderStageManager->GetByName<IRenderStage> (link.GetStageName ())->GetFramebuffer ();
    }

    if (link.GetRelative () == EPipelineLink::LINK_PREVIOUS)
    {
        if (m_currentRenderStageIdx == 0)
        {
            LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Pipeline link to previous render stage from the first stage";
        }

        return m_renderStageManager->GetByHandle<IRenderStage> (m_renderPipeline[m_currentRenderStageIdx - 1])->GetFramebuffer ();
    }

    /* LINK_CURRENT */
    return m_renderStageManager->GetByHandle<IRenderStage> (m_renderPipeline[m_currentRenderStageIdx])->GetFramebuffer ();
}

void Renderer::RenderFrame ()
{
    UpdateInvalidatedObjects ();

    // load per-frame data shared by render stages
    auto camera = GetGameScene ()->GetActiveCamera ();
    if (camera != nullptr)
    {
        UpdateCameraBuffers (camera);

        if (m_isShadowMapping)
        {
            UpdateLightViewBuffers ();
        }
    }

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
        auto shadow = this->Create<ShadowMapRenderStage> (RenderStageNames::ShadowMap);
        shadow->SetFaceCullingEnabled (true);
        shadow->SetFaceCullingMode (EFaceCullingFace::FACE_FRONT, EFaceCullingDirection::DIR_CCW);
        shadow->Initialize ();
    }

    if (m_isDeferredRendering == true)
    {
        // geometry stage
        auto baseDeferred = this->Create<DeferredGeometryRenderStage> (RenderStageNames::DeferredGeometry);
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
        auto baseForward = this->Create<ForwardGeometryRenderStage> (RenderStageNames::Forward);
        if (m_isShadowMapping == true)
        {
            baseForward->SetDepthTextureArrayFramebufferLink (RenderStageNames::ShadowMap);
            baseForward->SetDepthCubeMapArrayFramebufferLink (RenderStageNames::ShadowMap);
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


void Renderer::Update (std::shared_ptr<Material> material)
{
    handle_t shaderProgramHandle = m_shaderProgramManager->GetByName<ShaderProgram>(material->GetDeferredLightingPassShaderProgram ())->GetHandle ();
    std::string shaderProgramName = material->GetDeferredLightingPassShaderProgram ();

    if (m_isDeferredRendering)
    {
        // add material's shader program to set of used shader programs handles
        // add lighting deferred pass renderStages for each program
        if (m_lightingShaders.find (shaderProgramHandle) == m_lightingShaders.end ())
        {
            // the first lighting stage owns the framebuffer which all lighting stages draw to,
            // the other stages add their results to it (each stage lights only pixels of its own shader program)
            bool isFirstLightingStage = (m_lightingShaderStagesCount == 0);

            // lighting stages are placed right after the geometry stage and the previous lighting stages
            handle_t anchor = isFirstLightingStage ? m_renderStageManager->GetByName<RenderStage> (RenderStageNames::DeferredGeometry)->GetHandle () : m_lastLightingStageHandle;

            m_lightingShaderStagesCount++;
            m_lightingShaders.insert (shaderProgramHandle);

            std::string stageName = isFirstLightingStage ? std::string (RenderStageNames::DeferredLighting) : std::string (RenderStageNames::DeferredLighting) + "_" + shaderProgramName;
            auto q = Create <DeferredLightingRenderStage> (stageName);
            q->SetShaderProgram (shaderProgramName);
            q->SetStencilTestEnabled (true)->SetStencilTest (EStencilTestFunction::FUNCTION_EQUAL, static_cast<int> (shaderProgramHandle));
            q->SetClearColorOnFrameEnabled (isFirstLightingStage);
            q->SetClearDepthOnFrameEnabled (false);
            q->SetClearStencilOnFrameEnabled (false);
            q->SetDepthTestEnabled (false);
            q->SetColorAttachmentsFramebufferLink (RenderStageNames::DeferredGeometry);
            q->SetDepthStencilFramebufferLink (RenderStageNames::DeferredGeometry);
            q->SetDepthTextureArrayFramebufferLink (m_isShadowMapping ? PipelineLink (RenderStageNames::ShadowMap) : PipelineLink (EPipelineLink::LINK_CURRENT));
            q->SetDepthCubeMapArrayFramebufferLink (m_isShadowMapping ? PipelineLink (RenderStageNames::ShadowMap) : PipelineLink (EPipelineLink::LINK_CURRENT));
            q->SetDrawFramebufferLink (RenderStageNames::DeferredLighting);
            q->SetFramebufferEnabled (isFirstLightingStage);

            q->Initialize ();

            MoveRenderStageAfter (q->GetHandle (), anchor);
            m_lastLightingStageHandle = q->GetHandle ();
        }
    }
}

void Renderer::SubscribeToSceneMessages ()
{
    // set callback for new MeshObjects
    GetGameScene ()->GetGame ()->GetMessageBus ()->Subscribe<MeshObjectUpdateMessage> (
        [&](const std::shared_ptr<MeshObjectUpdateMessage>& message) 
        { 
            Update (GetGameScene ()->GetGameObjectManager ()->GetByHandle<MeshObject> (message->GetHandle ()));
            UpdateAABBBuffers (GetGameScene ()->GetGameObjectManager ()->GetByHandle<MeshObject> (message->GetHandle ()));
        }
    );

    // set callback for new or modified materials
    GetGameScene ()->GetGame ()->GetMessageBus ()->Subscribe<MaterialTextureUpdateMessage> (
        [&](const std::shared_ptr<MaterialTextureUpdateMessage>& message) 
        { 
            Update (GetGameScene ()->GetMaterialManager ()->GetByHandle<Material> (message->GetHandle ()), message->GetTextureUnit ());
        }
    );
    GetGameScene ()->GetGame ()->GetMessageBus ()->Subscribe<MaterialUpdateMessage> (
        [&](const std::shared_ptr<MaterialUpdateMessage>& message) 
        { 
            Update (GetGameScene ()->GetMaterialManager ()->GetByHandle<Material> (message->GetHandle ()));
        }
    );
    
    // set callback for new or modified lights
    GetGameScene ()->GetGame ()->GetMessageBus ()->Subscribe<LightUpdateMessage> (
        [&](const std::shared_ptr<LightUpdateMessage>& message) 
        { 
            GetGameScene ()->GetGameObjectManager ()->GetByHandle<GameObject> (message->GetHandle ())->OnUpdate (*this); 
        }
    );

    // set callback for modified scene graph (currently this only requires to reload light buffers)
    GetGameScene ()->GetGame ()->GetMessageBus ()->Subscribe<SceneGraphUpdateMessage> (
        [&](const std::shared_ptr<SceneGraphUpdateMessage>& message) 
        { 
            UpdateLightsRecursive (message->GetHandle ());
        }
    );

    // set callback for modified transforms (reload light buffers, reload AABB geometry buffers)
    GetGameScene ()->GetGame ()->GetMessageBus ()->Subscribe<TransformUpdateMessage> (
        [&](const std::shared_ptr<TransformUpdateMessage>& message) 
        { 
            m_invalidatedObjects.insert (message->GetHandle ());
        }
    );
    
}

void Renderer::UpdateInvalidatedObjects ()
{
    for (auto handle : m_invalidatedObjects)
    {
        // lights
        UpdateLightsRecursive (handle);

        // AABBs
        if (std::dynamic_pointer_cast<MeshObject> (GetGameScene ()->GetGameObjectManager ()->GetByHandle<GameObject> (handle)) != nullptr)
        {
            UpdateAABBBuffers (GetGameScene ()->GetGameObjectManager ()->GetByHandle<MeshObject> (handle));
        }
    }
}

void Renderer::UpdateLightsRecursive (handle_t objectHandle)
{
    auto light = GetGameScene ()->GetGameObjectManager ()->GetByHandle<GameObject> (objectHandle);

    if (std::dynamic_pointer_cast<Light>(light) != nullptr)
    {
        light->OnUpdate (*this);
    }

    for (auto&& childObject : light->GetChildren ())
    {
        UpdateLightsRecursive (childObject.lock ()->GetHandle ());
    }

}

void Renderer::MoveRenderStageAfter (handle_t stageHandle, handle_t anchorHandle)
{
    m_renderPipeline.erase (std::remove (m_renderPipeline.begin (), m_renderPipeline.end (), stageHandle), m_renderPipeline.end ());

    auto anchor = std::find (m_renderPipeline.begin (), m_renderPipeline.end (), anchorHandle);
    if (anchor == m_renderPipeline.end ())
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Render stage to place after is not in the pipeline";
    }

    m_renderPipeline.insert (anchor + 1, stageHandle);
}
} // namespace cilantro
