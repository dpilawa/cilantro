#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include "graphics/ShaderProgram.h"
#include "graphics/RenderStage.h"
#include "graphics/Framebuffer.h"
#include <string>
#include <vector>

class GameScene;

class Renderer
{
public:
    __EAPI Renderer (GameScene* gameScene, unsigned int width, unsigned int height);
    __EAPI virtual ~Renderer ();

    __EAPI virtual void Initialize ();
    __EAPI virtual void Deinitialize ();
    
    // scene
    GameScene* GetGameScene ();

    // dimensions
    __EAPI unsigned int GetWidth () const;
    __EAPI unsigned int GetHeight () const;
    __EAPI Renderer& SetResolution (unsigned int width, unsigned int height);

    // render
    __EAPI virtual void RenderFrame ();

    // framebuffer control
    virtual Framebuffer* CreateFramebuffer (unsigned int rgbTextures, unsigned int rgbaTextures, bool multisampleEnabled) = 0;
    virtual void BindFramebuffer (Framebuffer* framebuffer) = 0;
    virtual void BindFramebufferRenderbuffer (Framebuffer* framebuffer) = 0;

    virtual void ClearColorBuffer (Vector4f& rgba) = 0;
    virtual void ClearDepthBuffer () = 0;

    virtual void SetDepthTestEnabled (bool value) = 0;
    virtual void SetFaceCullingEnabled (bool value) = 0;
    virtual void SetMultisamplingEnabled (bool value) = 0;
    
    virtual void SetStencilTestEnabled (bool value) = 0;
    virtual void SetStencilTestFunction (StencilTestFunction testFunction, int testValue) = 0;

    // pipeline manipulation
    Renderer& RotateRenderPipelineLeft ();
    Renderer& RotateRenderPipelineRight ();
    std::vector<handle_t>& GetRenderPipeline ();
    virtual Framebuffer* GetPipelineFramebuffer (PipelineLink link);
    __EAPI virtual ResourceManager<RenderStage>& GetRenderStageManager ();
    
    template <typename T, typename ...Params>
    T& AddRenderStage (const std::string& name, Params&&... params);

    // shader library manipulation
    __EAPI virtual ResourceManager<ShaderProgram>& GetShaderProgramManager ();
    
    template <typename T, typename ...Params>
    T& AddShaderProgram (const std::string& name, Params&&... params);        

protected:

    GameScene* gameScene;

    unsigned int renderStage;
    ResourceManager<RenderStage> renderStages;
    std::vector<handle_t> renderPipeline;

    ResourceManager<ShaderProgram> shaderPrograms;

    unsigned int width;
    unsigned int height;

private:

    long int totalRenderFrames;
    float totalRenderTime;
    float totalFrameRenderTime;
};

template <typename T, typename ...Params>
T& Renderer::AddRenderStage (const std::string& name, Params&&... params)
{
    T& renderStage = renderStages.Create<T> (name, params...);
    renderStage.renderer = this;

    // initialize
    renderStage.Initialize ();
    renderPipeline.push_back (renderStage.GetHandle ());

    // return stage
    return renderStage;
}

template <typename T, typename ...Params>
T& Renderer::AddShaderProgram (const std::string& name, Params&&... params)
{
    T& shaderProgram = shaderPrograms.Create<T> (name, params...);

    // return program
    return shaderProgram;
}

#endif
