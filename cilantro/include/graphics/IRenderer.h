#pragma once

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include <set>
#include <vector>

struct IFramebuffer;
class CRenderStage;
class GameScene;
class MeshObject;
class Material;
class PointLight;
class DirectionalLight;
class SpotLight;
class Camera;
class ShaderProgram;
class Vector4f;

enum EPipelineLink { LINK_FIRST, LINK_SECOND, LINK_PREVIOUS, LINK_CURRENT, LINK_LAST };
enum EStencilTestFunction { FUNCTION_NEVER, FUNCTION_LESS, FUNCTION_LEQUAL, FUNCTION_GREATER, FUNCTION_GEQUAL, FUNCTION_EQUAL, FUNCTION_NOTEQUAL, FUNCTION_ALWAYS };

typedef CResourceManager<ShaderProgram> TShaderProgramManager;
typedef CResourceManager<CRenderStage> TRenderStageManager;
typedef std::vector<handle_t> TRenderPipeline;
typedef std::set<handle_t> TLightingShaderSet;

struct IRenderer
{
    virtual ~IRenderer() {};

    // initialize and cleanup
    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    // get and set rendering resolution
    virtual unsigned int GetWidth () const = 0;
    virtual unsigned int GetHeight () const = 0;
    virtual IRenderer& SetResolution (unsigned int width, unsigned int height) = 0;
    
    // access scene
    virtual GameScene* GetGameScene () = 0;

    // shader library manipulation
    virtual TShaderProgramManager& GetShaderProgramManager () = 0;

    // render pipeline
    virtual TRenderStageManager& GetRenderStageManager () = 0;
    
    virtual TRenderPipeline& GetRenderPipeline () = 0;
    virtual IRenderer& RotateRenderPipelineLeft () = 0;
    virtual IRenderer& RotateRenderPipelineRight () = 0;
    virtual IFramebuffer* GetPipelineFramebuffer (EPipelineLink link) = 0;    
    
    // render current frame
    virtual void RenderFrame () = 0;    

    // geometry
    virtual void Draw (MeshObject& meshObject) = 0;
    virtual void DrawViewportQuad (unsigned int x, unsigned int y, unsigned int width, unsigned int height) = 0;

    virtual void Update (MeshObject& meshObject) = 0;
    virtual void Update (Material& material, unsigned int textureUnit) = 0;
    virtual void Update (Material& material) = 0;
    
    virtual void Update (PointLight& pointLight) = 0;
    virtual void Update (DirectionalLight& directionalLight) = 0;	
    virtual void Update (SpotLight& spotLight) = 0;

    virtual void UpdateCameraBuffers (Camera& camera) = 0;

    // framebuffer control
    virtual IFramebuffer* CreateFramebuffer (unsigned int rgbTextures, unsigned int rgbaTextures, bool multisampleEnabled) = 0;
    virtual void BindDefaultFramebuffer () = 0;

    virtual void ClearColorBuffer (const Vector4f& rgba) = 0;
    virtual void ClearDepthBuffer () = 0;
    virtual void ClearStencilBuffer () = 0;

    virtual void SetDepthTestEnabled (bool value) = 0;
    virtual void SetFaceCullingEnabled (bool value) = 0;
    virtual void SetMultisamplingEnabled (bool value) = 0;
    
    virtual void SetStencilTestEnabled (bool value) = 0;
    virtual void SetStencilTestFunction (EStencilTestFunction testFunction, int testValue) = 0;
};