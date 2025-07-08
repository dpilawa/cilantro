#pragma once

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include <set>
#include <vector>

namespace cilantro {

struct IRenderStage;
struct IFramebuffer;
struct IShaderProgram;
class RenderStage;
class ShaderProgram;
class GameScene;
class MeshObject;
class Material;
class PointLight;
class DirectionalLight;
class SpotLight;
class Camera;
class AABB;

class Vector4f;

enum class EPipelineLink { LINK_FIRST, LINK_SECOND, LINK_THIRD, LINK_PREVIOUS, LINK_PREVIOUS_MINUS_1, LINK_CURRENT, LINK_LAST };
enum class EDepthTestFunction { FUNCTION_NEVER, FUNCTION_LESS, FUNCTION_LEQUAL, FUNCTION_GREATER, FUNCTION_GEQUAL, FUNCTION_EQUAL, FUNCTION_NOTEQUAL, FUNCTION_ALWAYS };
enum class EStencilTestFunction { FUNCTION_NEVER, FUNCTION_LESS, FUNCTION_LEQUAL, FUNCTION_GREATER, FUNCTION_GEQUAL, FUNCTION_EQUAL, FUNCTION_NOTEQUAL, FUNCTION_ALWAYS };
enum class EStencilTestOperation { OP_KEEP, OP_ZERO, OP_REPLACE, OP_INC, OP_INC_WRAP, OP_DEC, OP_DEC_WRAP, OP_INV };
enum class EFaceCullingFace { FACE_FRONT, FACE_BACK };
enum class EFaceCullingDirection { DIR_CW, DIR_CCW };

typedef ResourceManager<ShaderProgram> TShaderProgramManager;
typedef ResourceManager<RenderStage> TRenderStageManager;
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
    virtual std::shared_ptr<IRenderer> SetResolution (unsigned int width, unsigned int height) = 0;
    virtual std::shared_ptr<IRenderer> SetViewport (unsigned int x, unsigned int y, unsigned int sx, unsigned int sy) = 0;

    // access scene
    virtual std::shared_ptr<GameScene> GetGameScene () = 0;

    // shader library manipulation
    virtual std::shared_ptr<TShaderProgramManager> GetShaderProgramManager () = 0;

    // render pipeline
    virtual std::shared_ptr<TRenderStageManager> GetRenderStageManager () = 0;
    
    virtual std::shared_ptr<IRenderStage> GetCurrentRenderStage () = 0;
    virtual TRenderPipeline& GetRenderPipeline () = 0;
    virtual std::shared_ptr<IRenderer> RotateRenderPipelineLeft () = 0;
    virtual std::shared_ptr<IRenderer> RotateRenderPipelineRight () = 0;
    virtual std::shared_ptr<IFramebuffer> GetPipelineFramebuffer (EPipelineLink link) = 0;    
    
    // render current frame
    virtual void RenderFrame () = 0;

    // geometry
    virtual void Draw (std::shared_ptr<MeshObject> meshObject) = 0;
    virtual void DrawSurface () = 0;
    virtual void DrawSceneGeometryBuffers (std::shared_ptr<IShaderProgram> shader) = 0;
    virtual void DrawAABBGeometryBuffers (std::shared_ptr<IShaderProgram> shader) = 0;

    virtual void Update (std::shared_ptr<MeshObject> meshObject) = 0;
    virtual void UpdateAABBBuffers (std::shared_ptr<MeshObject> meshObject) = 0;
    virtual AABB CalculateAABB (std::shared_ptr<MeshObject> meshObject) = 0;
    virtual void Update (std::shared_ptr<Material>, unsigned int textureUnit) = 0;
    virtual void Update (std::shared_ptr<Material> material) = 0;
    
    virtual void Update (std::shared_ptr<PointLight> pointLight) = 0;
    virtual void Update (std::shared_ptr<DirectionalLight> directionalLight) = 0;	
    virtual void Update (std::shared_ptr<SpotLight> spotLight) = 0;

    virtual void UpdateCameraBuffers (std::shared_ptr<Camera> camera) = 0;
    virtual void UpdateLightViewBuffers () = 0;

    // object counts
    virtual size_t GetPointLightCount () const = 0;
    virtual size_t GetDirectionalLightCount () const = 0;
    virtual size_t GetSpotLightCount () const = 0;

    // rendering properties
    virtual bool IsDeferredRendering () const = 0;
    virtual bool IsShadowMapping () const = 0;

    // framebuffer control
    virtual std::shared_ptr<IFramebuffer> CreateFramebuffer (unsigned int width, unsigned int height, unsigned int rgbTextureCount, unsigned int rgbaTextureCount, unsigned int depthBufferArrayTextureCount, bool depthStencilRenderbufferEnabled, bool multisampleEnabled) = 0;
    virtual void BindDefaultFramebuffer () = 0;
    virtual void BindDefaultDepthBuffer () = 0;
    virtual void BindDefaultStencilBuffer () = 0;
    virtual void BindDefaultTextures () = 0;

    virtual void ClearColorBuffer (const Vector4f& rgba) = 0;
    virtual void ClearDepthBuffer () = 0;
    virtual void ClearStencilBuffer () = 0;

    virtual void SetDepthTestEnabled (bool value) = 0;
    virtual void SetDepthTestFunction (EDepthTestFunction depthTestFunction) = 0;
    virtual void SetFaceCullingEnabled (bool value) = 0;
    virtual void SetFaceCullingMode (EFaceCullingFace face, EFaceCullingDirection direction) = 0;
    virtual void SetMultisamplingEnabled (bool value) = 0;
    
    virtual void SetStencilTestEnabled (bool value) = 0;
    virtual void SetStencilTestFunction (EStencilTestFunction testFunction, int testValue) = 0;
    virtual void SetStencilTestOperation (EStencilTestOperation sFail, EStencilTestOperation dpFail, EStencilTestOperation dpPass) = 0;
};

} // namespace cilantro