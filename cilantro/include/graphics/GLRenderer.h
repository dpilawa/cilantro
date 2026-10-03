#pragma once

#include "cilantroengine.h"
#include "graphics/Renderer.h"
#include "graphics/GLTypes.h"
#include "math/AABB.h"

namespace cilantro {

class GameScene;
class MeshObject;
class Camera;
class GLCameraBuffer;
class GLLightBuffers;
class GLGeometryStore;
class GLMaterialBindings;
class GLShaderLibrary;

class __CEAPI GLRenderer : public Renderer
{
public:
    __EAPI GLRenderer (std::shared_ptr<GameScene> gameScene, unsigned int width, unsigned int height, bool shadowMappingEnabled, bool deferredRenderingEnabled);
    __EAPI virtual ~GLRenderer ();

    ///////////////////////////////////////////////////////////////////////////

    __EAPI virtual void Initialize () override;
    __EAPI virtual void Deinitialize () override;
    
    __EAPI virtual std::shared_ptr<IRenderer> SetViewport (unsigned int x, unsigned int y, unsigned int sx, unsigned int sy) override;
    
    
    __EAPI virtual void Draw (std::shared_ptr<MeshObject> meshObject) override;
    __EAPI virtual void DrawSurface () override;
    __EAPI virtual void DrawSceneGeometryBuffers (std::shared_ptr<IShaderProgram> shader) override;
    __EAPI virtual void DrawAABBGeometryBuffers (std::shared_ptr<IShaderProgram> shader) override;
    
    __EAPI virtual void Update (std::shared_ptr<MeshObject> meshObject) override;
    __EAPI virtual void UpdateAABBBuffers (std::shared_ptr<MeshObject> meshObject) override;
    __EAPI virtual AABB CalculateAABB (std::shared_ptr<MeshObject> meshObject) override;

    __EAPI virtual void Update (std::shared_ptr<Material> material, unsigned int textureUnit) override;
    
    __EAPI virtual void Update (std::shared_ptr<PointLight> pointLight) override;
    __EAPI virtual void Update (std::shared_ptr<DirectionalLight> directionalLight) override;    
    __EAPI virtual void Update (std::shared_ptr<SpotLight> spotLight) override;
    
    __EAPI virtual void UpdateCameraBuffers (std::shared_ptr<Camera> camera) override;
    __EAPI virtual void UpdateLightViewBuffers () override;
    
    __EAPI virtual size_t GetPointLightCount () const override;
    __EAPI virtual size_t GetDirectionalLightCount () const override;
    __EAPI virtual size_t GetSpotLightCount () const override;

    __EAPI virtual std::shared_ptr<IFramebuffer> CreateFramebuffer (unsigned int width, unsigned int height, unsigned int rgbTextureCount, unsigned int rgbaTextureCount, unsigned int depthBufferArrayTextureCount, bool depthStencilRenderbufferEnabled, bool multisampleEnabled) override;
    __EAPI virtual void BindDefaultFramebuffer () override;
    __EAPI virtual void BindDefaultDepthBuffer () override;
    __EAPI virtual void BindDefaultStencilBuffer () override;
    __EAPI virtual void BindDefaultTextures () override;    
    
    __EAPI virtual void ClearColorBuffer (const Vector4f& rgba) override;
    __EAPI virtual void ClearDepthBuffer () override;
    __EAPI virtual void ClearStencilBuffer () override;
    
    __EAPI virtual void SetDepthTestEnabled (bool value) override;
    __EAPI virtual void SetDepthTestFunction (EDepthTestFunction depthTestFunction) override;
    __EAPI virtual void SetFaceCullingEnabled (bool value) override;
    __EAPI virtual void SetFaceCullingMode (EFaceCullingFace face, EFaceCullingDirection direction) override;
    __EAPI virtual void SetMultisamplingEnabled (bool value) override;
    
    __EAPI virtual void SetStencilTestEnabled (bool value) override;
    __EAPI virtual void SetStencilTestFunction (EStencilTestFunction testFunction, int testValue) override;
    __EAPI virtual void SetStencilTestOperation (EStencilTestOperation sFail, EStencilTestOperation dpFail, EStencilTestOperation dpPass) override;

    ///////////////////////////////////////////////////////////////////////////

private:
    void InitializeObjectBuffers ();

    void InitializeLightUniformBuffers ();
    void DeinitializeLightUniformBuffers ();

private:
    std::unique_ptr<GLGeometryStore> m_geometryStore;

    // GL buffers and shaders shared by entire scene
    std::unique_ptr<GLCameraBuffer> m_cameraBuffer;
    std::unique_ptr<GLShaderLibrary> m_shaderLibrary;
    std::unique_ptr<GLLightBuffers> m_lightBuffers;
    std::unique_ptr<GLMaterialBindings> m_materialBindings;
};

} // namespace cilantro
