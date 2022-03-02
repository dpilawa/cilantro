#include "graphics/Renderer.h"
#include "graphics/RenderStage.h"
#include "graphics/GLDeferredGeometryRenderStage.h"
#include "graphics/GLForwardGeometryRenderStage.h"
#include "graphics/Framebuffer.h"
#include "scene/GameScene.h"
#include "system/Timer.h"
#include "system/LogMessage.h"
#include <cmath>

Renderer::Renderer (GameScene* gameScene, unsigned int width, unsigned int height, bool isDeferred)
    : m_IsDeferred(isDeferred)
{
    totalRenderFrames = 0L;
    totalRenderTime = 0.0f;
    totalFrameRenderTime = 0.0f;

    this->gameScene = gameScene;
    this->quadGeometryBuffer = nullptr;

    this->width = width;
    this->height = height;
}

Renderer::~Renderer ()
{
}

void Renderer::Initialize ()
{
    InitializeObjectBuffers ();
    InitializeRenderStages ();
    InitializeMatrixUniformBuffers ();
    InitializeLightUniformBuffers ();

    for (auto&& stage : renderStages)
    {
        stage->Initialize ();
    }

    // set callback for new MeshObjects
    gameScene->RegisterCallback ("OnUpdateMeshObject", [&](unsigned int objectHandle, unsigned int) { gameScene->GetGameObjectManager ().GetByHandle<GameObject> (objectHandle).OnUpdate (*this); });

    // set callback for new or modified materials
    gameScene->RegisterCallback ("OnUpdateMaterialTexture", [&](unsigned int materialHandle, unsigned int textureUnit) { Update (gameScene->GetMaterialManager ().GetByHandle<Material> (materialHandle), textureUnit); });
    gameScene->RegisterCallback ("OnUpdateMaterial", [&](unsigned int materialHandle, unsigned int) { Update (gameScene->GetMaterialManager ().GetByHandle<Material> (materialHandle)); });
    
    // set callback for new or modified lights
    gameScene->RegisterCallback ("OnUpdateLight", [&](unsigned int objectHandle, unsigned int) { gameScene->GetGameObjectManager ().GetByHandle<GameObject> (objectHandle).OnUpdate (*this); });

    // set callback for modified scene graph (currently this only requires to reload light buffers)
    gameScene->RegisterCallback ("OnUpdateSceneGraph", [&](unsigned int objectHandle, unsigned int) { UpdateLightBufferRecursive (objectHandle); });

    // set callback for modified transforms (currently this only requires to reload light buffers)
    gameScene->RegisterCallback ("OnUpdateTransform", [&](unsigned int objectHandle, unsigned int) { UpdateLightBufferRecursive (objectHandle); });

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

std::unordered_map <handle_t, SGeometryBuffers*>& Renderer::GetSceneGeometryBufferMap ()
{
    return sceneGeometryBuffers;
}

SGeometryBuffers* Renderer::GetQuadGeometryBuffer () const
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

void Renderer::InitializeObjectBuffers ()
{
    // create and load object buffers for all existing objects
    for (auto&& gameObject : gameScene->GetGameObjectManager ())
    {
        // load buffers for MeshObject only
        if (std::dynamic_pointer_cast<MeshObject> (gameObject) != nullptr)
        {
            gameObject->OnUpdate (*this);
        }      
    }
}

void Renderer::InitializeRenderStages ()
{
    if (m_IsDeferred == true)
    {
        // geometry stage
        this->AddRenderStage<GLDeferredGeometryRenderStage> ("base");
        
        // lighting stages (per material shader)
        for (auto&& material : gameScene->GetMaterialManager ())
        {
            this->Update (*material);
        }
    }
    else
    {
        this->AddRenderStage<GLForwardGeometryRenderStage> ("base");
    }
}