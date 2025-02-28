#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include "graphics/IRenderer.h"
#include "graphics/IRenderStage.h"
#include <string>
#include <vector>
#include <set>

class CGameScene;

class CRenderer : public IRenderer
{
public:
    CRenderer (CGameScene* gameScene, unsigned int width, unsigned int height, bool shadowMappingEnabled, bool deferredRenderingEnabled);
    virtual ~CRenderer () {};

    ///////////////////////////////////////////////////////////////////////////

    void Initialize () override;
    void Deinitialize () override;

    __EAPI unsigned int GetWidth () const override final;
    __EAPI unsigned int GetHeight () const override final;
    __EAPI IRenderer& SetResolution (unsigned int width, unsigned int height) override;

    CGameScene* GetGameScene () override final;

    __EAPI virtual TShaderProgramManager& GetShaderProgramManager () override final;

    __EAPI virtual TRenderStageManager& GetRenderStageManager () override final;
    
    virtual IRenderStage* GetCurrentRenderStage () override final;
    virtual TRenderPipeline& GetRenderPipeline () override final;
    virtual IRenderer& RotateRenderPipelineLeft () override final;
    virtual IRenderer& RotateRenderPipelineRight () override final;
    virtual IFramebuffer* GetPipelineFramebuffer (EPipelineLink link) override final;
    
    virtual void RenderFrame () override;   
    
    ///////////////////////////////////////////////////////////////////////////

    template <typename T, typename ...Params>
    T& AddRenderStage (const std::string& name, Params&&... params);
    
    template <typename T, typename ...Params>
    T& AddShaderProgram (const std::string& name, Params&&... params);        

protected:
    // game scene being rendered
    CGameScene* m_gameScene;

    // render pipeline
    size_t m_currentRenderStageIdx;
    IRenderStage* m_currentRenderStage;
    TRenderStageManager m_renderStageManager;
    TRenderPipeline m_renderPipeline;

    // shader library
    TShaderProgramManager m_shaderProgramManager;

    // set of handles of distinct lighting pass shader programs used in the scene
    TLightingShaderSet m_lightingShaders;
    size_t m_lightingShaderStagesCount;

    // dimensions
    unsigned int m_width;
    unsigned int m_height;

    // flags
    bool m_isDeferredRendering;
    bool m_isShadowMapping;

private:
    // timing data
    long int m_totalRenderedFrames;
    float m_totalRenderTime;
    float m_totalFrameRenderTime;

    // initialize and deinitialize all required internal renderstages
    void InitializeRenderStages ();
    void DeinitializeRenderStages ();
};

template <typename T, typename ...Params>
T& CRenderer::AddRenderStage (const std::string& name, Params&&... params)
{
    T& renderStage = m_renderStageManager.Create<T> (name, params...);
    renderStage.m_renderer = this;

    // initialize
    renderStage.Initialize ();
    m_renderPipeline.push_back (renderStage.GetHandle ());

    // return stage
    return renderStage;
}

template <typename T, typename ...Params>
T& CRenderer::AddShaderProgram (const std::string& name, Params&&... params)
{
    T& shaderProgram = m_shaderProgramManager.Create<T> (name, params...);

    // return program
    return shaderProgram;
}

#endif
