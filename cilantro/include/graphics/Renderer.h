#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include "graphics/IRenderer.h"
#include "graphics/RenderStage.h"
#include <string>
#include <vector>
#include <set>

class GameScene;

class CRenderer : public IRenderer
{
public:
    CRenderer (GameScene* gameScene, unsigned int width, unsigned int height, bool isDeferred);
    virtual ~CRenderer () {};

    ///////////////////////////////////////////////////////////////////////////

    void Initialize () override;
    void Deinitialize () override;

    __EAPI unsigned int GetWidth () const override final;
    __EAPI unsigned int GetHeight () const override final;
    __EAPI IRenderer& SetResolution (unsigned int width, unsigned int height) override;

    GameScene* GetGameScene () override final;

    __EAPI virtual TShaderProgramManager& GetShaderProgramManager () override final;

    __EAPI virtual TRenderStageManager& GetRenderStageManager () override final;
    
    virtual TRenderPipeline& GetRenderPipeline () override final;
    virtual IRenderer& RotateRenderPipelineLeft () override final;
    virtual IRenderer& RotateRenderPipelineRight () override final;
    virtual Framebuffer* GetPipelineFramebuffer (EPipelineLink link) override final;
    
    virtual void RenderFrame () override;   
    
    ///////////////////////////////////////////////////////////////////////////

    template <typename T, typename ...Params>
    T& AddRenderStage (const std::string& name, Params&&... params);
    
    template <typename T, typename ...Params>
    T& AddShaderProgram (const std::string& name, Params&&... params);        

protected:
    // game scene being rendered
    GameScene* m_GameScene;

    // render pipeline
    unsigned int m_CurrentRenderStage;
    TRenderStageManager m_RenderStageManager;
    TRenderPipeline m_RenderPipeline;

    // shader library
    TShaderProgramManager m_ShaderProgramManager;

    // set of handles of distinct lighting pass shader programs used in the scene
    TLightingShaderSet m_LightingShaders;
    unsigned int m_LightingShaderStagesCount;

    // dimensions
    unsigned int m_Width;
    unsigned int m_Height;

    // flags
    bool m_IsDeferred;

private:
    // timing data
    long int m_TotalRenderedFrames;
    float m_TotalRenderTime;
    float m_TotalFrameRenderTime;

    // initialize and deinitialize all required internal renderstages
    void InitializeRenderStages ();
    void DeinitializeRenderStages ();
};

template <typename T, typename ...Params>
T& CRenderer::AddRenderStage (const std::string& name, Params&&... params)
{
    T& renderStage = m_RenderStageManager.Create<T> (name, params...);
    renderStage.renderer = this;

    // initialize
    renderStage.Initialize ();
    m_RenderPipeline.push_back (renderStage.GetHandle ());

    // return stage
    return renderStage;
}

template <typename T, typename ...Params>
T& CRenderer::AddShaderProgram (const std::string& name, Params&&... params)
{
    T& shaderProgram = m_ShaderProgramManager.Create<T> (name, params...);

    // return program
    return shaderProgram;
}

#endif
