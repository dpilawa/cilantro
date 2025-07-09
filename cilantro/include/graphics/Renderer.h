#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include "graphics/IRenderer.h"
#include "graphics/IRenderStage.h"
#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <memory>

namespace cilantro {

class GameScene;
class GameObject;

class __CEAPI Renderer : public IRenderer, public std::enable_shared_from_this<Renderer>
{
public:
    __EAPI Renderer (std::shared_ptr<GameScene> gameScene, unsigned int width, unsigned int height, bool shadowMappingEnabled, bool deferredRenderingEnabled);
    __EAPI virtual ~Renderer ();

    ///////////////////////////////////////////////////////////////////////////

    __EAPI void Initialize () override;
    __EAPI void Deinitialize () override;

    __EAPI unsigned int GetWidth () const override final;
    __EAPI unsigned int GetHeight () const override final;
    __EAPI std::shared_ptr<IRenderer> SetResolution (unsigned int width, unsigned int height) override;

    __EAPI std::shared_ptr<GameScene> GetGameScene () override final;

    __EAPI virtual std::shared_ptr<TShaderProgramManager> GetShaderProgramManager () override final;

    __EAPI virtual std::shared_ptr<TRenderStageManager> GetRenderStageManager () override final;
    
    __EAPI virtual std::shared_ptr<IRenderStage> GetCurrentRenderStage () override final;
    __EAPI virtual TRenderPipeline& GetRenderPipeline () override final;
    __EAPI virtual std::shared_ptr<IRenderer> RotateRenderPipelineLeft () override final;
    __EAPI virtual std::shared_ptr<IRenderer> RotateRenderPipelineRight () override final;
    __EAPI virtual std::shared_ptr<IFramebuffer> GetPipelineFramebuffer (EPipelineLink link) override final;

    __EAPI virtual void RenderFrame () override;   
    
    __EAPI virtual AABB CalculateAABB (std::shared_ptr<MeshObject> meshObject) override;

    ///////////////////////////////////////////////////////////////////////////

    __EAPI virtual bool IsDeferredRendering () const override;
    __EAPI virtual bool IsShadowMapping () const override;

    template <typename T, typename ...Params>
    std::shared_ptr<T> Create (const std::string& name, Params&&... params)
    requires (std::is_base_of_v<IRenderStage,T>);
    
    template <typename T, typename ...Params>
    std::shared_ptr<T> Create (const std::string& name, Params&&... params)
    requires (std::is_base_of_v<IShaderProgram,T>);        

protected:
    // game scene being rendered
    std::weak_ptr<GameScene> m_gameScene;

    // objects with invalidated transformation
    std::unordered_set<handle_t> m_invalidatedObjects;

    // render pipeline
    size_t m_currentRenderStageIdx;
    std::shared_ptr<IRenderStage> m_currentRenderStage;
    std::shared_ptr<TRenderStageManager> m_renderStageManager;
    TRenderPipeline m_renderPipeline;

    // shader library
    std::shared_ptr<TShaderProgramManager> m_shaderProgramManager;

    // set of handles of distinct lighting pass shader programs used in the scene
    TLightingShaderSet m_lightingShaders;
    size_t m_lightingShaderStagesCount;

    // dimensions
    unsigned int m_width;
    unsigned int m_height;

    // flags
    bool m_isDeferredRendering;
    bool m_isShadowMapping;

    // timing data
    long int m_totalRenderedFrames;
    long int m_totalDroppedFrames;
    float m_totalRenderTime;
    float m_totalFrameRenderTime;

private:

    // initialize and deinitialize all required internal renderstages
    void InitializeRenderStages ();
    void DeinitializeRenderStages ();
};

template <typename T, typename ...Params>
std::shared_ptr<T> Renderer::Create (const std::string& name, Params&&... params)
    requires (std::is_base_of_v<IRenderStage,T>)
{
    auto renderStage = m_renderStageManager->Create<T> (name, shared_from_this(), params...);

    // initialize
    renderStage->Initialize ();
    m_renderPipeline.push_back (renderStage->GetHandle ());

    // return stage
    return renderStage;
}

template <typename T, typename ...Params>
std::shared_ptr<T> Renderer::Create (const std::string& name, Params&&... params)
    requires (std::is_base_of_v<IShaderProgram,T>)
{
    auto shaderProgram = m_shaderProgramManager->Create<T> (name, params...);

    // return program
    return shaderProgram;
}

} // namespace cilantro

#endif
