#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include "graphics/RenderStage.h"
#include <string>
#include <vector>
#include <set>

class GameScene;
class Framebuffer;
class ShaderProgram;
class MeshObject;
class Material;
class PointLight;
class DirectionalLight;
class SpotLight;

enum EVBOType { VBO_VERTICES = 0, VBO_NORMALS, VBO_UVS, VBO_TANGENTS, VBO_BITANGENTS, VBO_BONES, VBO_BONEWEIGHTS };
enum EUBOType { UBO_MATRICES = 0, UBO_POINTLIGHTS, UBO_DIRECTIONALLIGHTS, UBO_SPOTLIGHTS };

struct SGeometryBuffers
{
    // number of vertices
    unsigned int vertexCount;
};

struct SUniformBuffers
{
};

struct SUniformMatrixBuffer
{
};

struct SMaterialTextureUnits
{
    // how many units in use 
    unsigned int unitsCount;
};

struct SUniformPointLightBuffer
{
};

struct SUniformDirectionalLightBuffer
{
};

struct SUniformSpotLightBuffer
{
};

class Renderer
{
public:
    __EAPI Renderer (GameScene* gameScene, unsigned int width, unsigned int height, bool isDeferred);
    __EAPI virtual ~Renderer ();

    __EAPI virtual void Initialize ();
    __EAPI virtual void Deinitialize ();

    // dimensions
    __EAPI unsigned int GetWidth () const;
    __EAPI unsigned int GetHeight () const;
    __EAPI Renderer& SetResolution (unsigned int width, unsigned int height);

    // render
    __EAPI virtual void RenderFrame ();
    __EAPI virtual void RenderGeometryBuffer (SGeometryBuffers* buffer) = 0;

    // scene
    GameScene* GetGameScene ();

    // geometry buffer accessors
    std::unordered_map <handle_t, SGeometryBuffers*>& GetSceneGeometryBufferMap ();
    SGeometryBuffers* GetQuadGeometryBuffer () const;

    // framebuffer control
    virtual Framebuffer* CreateFramebuffer (unsigned int rgbTextures, unsigned int rgbaTextures, bool multisampleEnabled) = 0;
    virtual void BindDefaultFramebuffer () = 0;

    virtual void ClearColorBuffer (const Vector4f& rgba) = 0;
    virtual void ClearDepthBuffer () = 0;
    virtual void ClearStencilBuffer () = 0;

    virtual void SetDepthTestEnabled (bool value) = 0;
    virtual void SetFaceCullingEnabled (bool value) = 0;
    virtual void SetMultisamplingEnabled (bool value) = 0;
    
    virtual void SetStencilTestEnabled (bool value) = 0;
    virtual void SetStencilTestFunction (StencilTestFunction testFunction, int testValue) = 0;

    // geometry
    virtual void Draw (MeshObject& meshObject) = 0;
    virtual void Update (MeshObject& meshObject) = 0;
    virtual void Update (Material& material, unsigned int textureUnit) = 0;
    virtual void Update (Material& material) = 0;
    
    virtual void Update (PointLight& pointLight) = 0;
    virtual void Update (DirectionalLight& directionalLight) = 0;	
    virtual void Update (SpotLight& spotLight) = 0;    

    virtual void LoadMatrixUniformBuffers () = 0;

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
    
    // buffers with geometry data to be passed to GPU (key is object handle)
    std::unordered_map <handle_t, SGeometryBuffers*> sceneGeometryBuffers;
    SGeometryBuffers* quadGeometryBuffer;

    // Buffers for uniforms shared by entire scene
    SUniformBuffers* uniformBuffers;

    // data structures for uniforms
    SUniformMatrixBuffer* uniformMatrixBuffer;
    SUniformPointLightBuffer* uniformPointLightBuffer;
    SUniformDirectionalLightBuffer* uniformDirectionalLightBuffer;
    SUniformSpotLightBuffer* uniformSpotLightBuffer;

    // materials texture units (key is material handle)
    std::unordered_map <unsigned int, SMaterialTextureUnits*> materialTextureUnits;

    // render pipeline
    unsigned int renderStage;
    ResourceManager<RenderStage> renderStages;
    std::vector<handle_t> renderPipeline;

    // shaders
    ResourceManager<ShaderProgram> shaderPrograms;

    // set of handles of distinct lighting pass shader programs used in the scene
    std::set <handle_t> lightingShaders;
    unsigned int lightingShaderStagesCount = 0;

    // dimensions
    unsigned int width;
    unsigned int height;

    // flags
    bool m_IsDeferred;

private:

    // timing data
    long int totalRenderFrames;
    float totalRenderTime;
    float totalFrameRenderTime;

    // initialize object buffers
    virtual void InitializeObjectBuffers () = 0;
    virtual void DeinitializeObjectBuffers () = 0;
    
    // initialize uniform buffers of view and projection matrices
    virtual void InitializeMatrixUniformBuffers () = 0;
    virtual void DeinitializeMatrixUniformBuffers () = 0;

    // initialize uniform buffers of lights
    virtual void InitializeLightUniformBuffers () = 0;
    virtual void DeinitializeLightUniformBuffers () = 0;
    virtual void UpdateLightBufferRecursive (unsigned int objectHandle) = 0;

    // initialize all required internal renderstages
    void InitializeRenderStages ();
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
