#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include "graphics/ShaderProgram.h"
#include "graphics/RenderStage.h"
#include <string>
#include <vector>

class MeshObject;
class PointLight;
class DirectionalLight;
class SpotLight;
class Material;
class Camera;

class Renderer
{
public:
    __EAPI Renderer (unsigned int width, unsigned int height);
    __EAPI virtual ~Renderer ();

    virtual void Initialize ();
    virtual void Deinitialize ();

    // dimensions
    __EAPI unsigned int GetWidth () const;
    __EAPI unsigned int GetHeight () const;
    __EAPI Renderer& SetResolution (unsigned int width, unsigned int height);

    // render
    __EAPI virtual void RenderFrame ();

    // pipeline manipulation
    __EAPI Renderer& RotateRenderPipelineLeft ();
    __EAPI Renderer& RotateRenderPipelineRight ();
    __EAPI std::vector<handle_t>& GetRenderPipeline ();
    __EAPI virtual Framebuffer* GetPipelineFramebuffer (PipelineLink link);
    __EAPI virtual ResourceManager<RenderStage>& GetRenderStageManager ();
    
    template <typename T, typename ...Params>
    T& AddRenderStage (const std::string& name, Params&&... params);

    // shader library manipulation
    __EAPI virtual ResourceManager<ShaderProgram>& GetShaderProgramManager ();
    
    template <typename T, typename ...Params>
    T& AddShaderProgram (const std::string& name, Params&&... params);        

protected:

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
