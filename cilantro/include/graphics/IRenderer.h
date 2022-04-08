#pragma once

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include <set>
#include <vector>

struct IFramebuffer;
struct IShaderProgram;
class CRenderStage;
class CGameScene;
class MeshObject;
class Material;
class PointLight;
class DirectionalLight;
class SpotLight;
class Camera;
class CShaderProgram;
class Vector4f;

enum EPipelineLink { LINK_FIRST, LINK_SECOND, LINK_PREVIOUS, LINK_CURRENT, LINK_LAST };
enum EStencilTestFunction { FUNCTION_NEVER, FUNCTION_LESS, FUNCTION_LEQUAL, FUNCTION_GREATER, FUNCTION_GEQUAL, FUNCTION_EQUAL, FUNCTION_NOTEQUAL, FUNCTION_ALWAYS };
enum EStencilTestOperation { OP_KEEP, OP_ZERO, OP_REPLACE, OP_INC, OP_INC_WRAP, OP_DEC, OP_DEC_WRAP, OP_INV };

typedef CResourceManager<CShaderProgram> TShaderProgramManager;
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
    virtual IRenderer& SetViewport (unsigned int x, unsigned int y, unsigned int sx, unsigned int sy) = 0;

    // access scene
    virtual CGameScene* GetGameScene () = 0;

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
    virtual void DrawQuad () = 0;
    virtual void DrawAllGeometryBuffers (IShaderProgram& shader) = 0;

    virtual void Update (MeshObject& meshObject) = 0;
    virtual void Update (Material& material, unsigned int textureUnit) = 0;
    virtual void Update (Material& material) = 0;
    
    virtual void Update (PointLight& pointLight) = 0;
    virtual void Update (DirectionalLight& directionalLight) = 0;	
    virtual void Update (SpotLight& spotLight) = 0;

    virtual void UpdateCameraBuffers (Camera& camera) = 0;
    virtual void UpdateLightViewBuffers () = 0;

    // object counts
    virtual size_t GetPointLightCount () const = 0;
    virtual size_t GetDirectionalLightCount () const = 0;
    virtual size_t GetSpotLightCount () const = 0;

    // framebuffer control
    virtual IFramebuffer* CreateFramebuffer (size_t width, size_t height, size_t rgbTextureCount, size_t rgbaTextureCount, size_t depthBufferArrayTextureCount, bool depthStencilRenderbufferEnabled, bool multisampleEnabled) = 0;
    virtual void BindDefaultFramebuffer () = 0;
    virtual void BindDefaultDepthBuffer () = 0;
    virtual void BindDefaultStencilBuffer () = 0;
    virtual void BindDefaultTextures () = 0;

    virtual void ClearColorBuffer (const Vector4f& rgba) = 0;
    virtual void ClearDepthBuffer () = 0;
    virtual void ClearStencilBuffer () = 0;

    virtual void SetDepthTestEnabled (bool value) = 0;
    virtual void SetFaceCullingEnabled (bool value) = 0;
    virtual void SetMultisamplingEnabled (bool value) = 0;
    
    virtual void SetStencilTestEnabled (bool value) = 0;
    virtual void SetStencilTestFunction (EStencilTestFunction testFunction, int testValue) = 0;
    virtual void SetStencilTestOperation (EStencilTestOperation sFail, EStencilTestOperation dpFail, EStencilTestOperation dpPass) = 0;
};