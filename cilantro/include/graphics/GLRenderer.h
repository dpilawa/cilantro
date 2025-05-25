#pragma once

#include "cilantroengine.h"
#include "glad/gl.h"
#include "graphics/Renderer.h"
#include "math/AABB.h"

namespace cilantro {

class GameScene;
class MeshObject;
class Camera;

enum EGlVBOType { VBO_VERTICES = 0, VBO_NORMALS, VBO_UVS, VBO_TANGENTS, VBO_BITANGENTS, VBO_BONES, VBO_BONEWEIGHTS };
enum EGlUBOType { UBO_MATRICES = 0, UBO_POINTLIGHTS, UBO_DIRECTIONALLIGHTS, UBO_SPOTLIGHTS, UBO_DIRECTIONALLIGHTVIEWMATRICES, UBO_BONETRANSFORMATIONS };
enum EGlSSBOType { SSBO_VERTICES = 0, SSBO_BONEINDICES, SSBO_BONEWEIGHTS, SSBO_AABB };

struct SGlGeometryBuffers;
struct SGlMaterialTextureUnits;

typedef std::unordered_map <handle_t, SGlGeometryBuffers*> TObjectGeometryBufferMap;
typedef std::unordered_map <handle_t, SGlMaterialTextureUnits*> TMaterialTextureUnitsMap;
typedef std::unordered_map <handle_t, size_t> TLightHandleIdxMap;

struct SGlGeometryBuffers
{
    // number of vertices
    size_t indexCount;
    // Vertex Buffer Objects (vertices, normals, uvs, tangents, bitangents, bone indices, bone weights)
    GLuint VBO[CILANTRO_VBO_COUNT];
    // Element Buffer Object (face indices)
    GLuint EBO;
    // Vertex Array Object
    GLuint VAO;
    // Bone transformation buffers
    GLuint vertexPositionsSSBO;
    GLuint boneTransformationsUBO;
    GLuint boneIndicesSSBO;
    GLuint boneWeightsSSBO;
    GLuint aabbSSBO;
};

struct SGlUniformBuffers
{
    // Uniform Buffer Objects (view & projection matrices, point lights, directional lights, spot lights, directional light view transforms)
    GLuint UBO[CILANTRO_GLOBAL_UBO_COUNT];
};

struct SGlUniformMatrixBuffer
{
    // view matrix
    GLfloat viewMatrix[16];
    // projection matrix
    GLfloat projectionMatrix[16];
};

struct SGlUniformLightViewMatrixBuffer
{
    // directional light view matrices
    GLfloat directionalLightView[16 * CILANTRO_MAX_DIRECTIONAL_LIGHTS];
};

struct SGlMaterialTextureUnits
{
    // how many units in use 
    unsigned int unitsCount;
    // using 16 texture units, as per minimum defined in OpenGL 3.x
    GLuint textureUnits[CILANTRO_MAX_TEXTURE_UNITS];
};

struct SGlPointLightStruct
{
    GLfloat lightPosition[3];
    GLfloat pad1;
    GLfloat lightColor[3];
    GLfloat attenuationConst;
    GLfloat attenuationLinear;
    GLfloat attenuationQuadratic;
};

struct SGlDirectionalLightStruct
{
    GLfloat lightDirection[3];
    GLfloat pad1;
    GLfloat lightColor[3];
    GLfloat pad2;
};

struct SGlSpotLightStruct
{
    GLfloat lightPosition[3];
    GLfloat pad1;
    GLfloat lightDirection[3];
    GLfloat pad2;
    GLfloat lightColor[3];
    GLfloat attenuationConst;
    GLfloat attenuationLinear;
    GLfloat attenuationQuadratic;
    GLfloat innerCutoffCosine;
    GLfloat outerCutoffCosine;
};

struct SGlUniformPointLightBuffer
{
    // number of active point lights
    GLuint pointLightCount;
    // pad to std140 specification
    GLint pad[3];
    // array of active point lights
    SGlPointLightStruct pointLights[CILANTRO_MAX_POINT_LIGHTS];
};

struct SGlUniformDirectionalLightBuffer
{
    // number of active directional lights
    GLuint directionalLightCount;
    // pad to std140 specification
    GLint pad[3];
    // array of active point lights
    SGlDirectionalLightStruct directionalLights[CILANTRO_MAX_DIRECTIONAL_LIGHTS];
};

struct SGlUniformSpotLightBuffer
{
    // number of active spot lights
    GLuint spotLightCount;
    // pad to std140 specification
    GLint pad[3];
    // array of active point lights
    SGlSpotLightStruct spotLights[CILANTRO_MAX_SPOT_LIGHTS];
};

struct SGlEncodedAABB {
    GLuint minBits[3];
    GLuint pad1;
    GLuint maxBits[3];
    GLuint pad2;
};

class __CEAPI GLRenderer : public Renderer
{
public:
    __EAPI GLRenderer (std::shared_ptr<GameScene> gameScene, unsigned int width, unsigned int height, bool shadowMappingEnabled, bool deferredRenderingEnabled);
    __EAPI virtual ~GLRenderer ();

    ///////////////////////////////////////////////////////////////////////////

    __EAPI virtual void Initialize () override;
    __EAPI virtual void Deinitialize () override;
    
    __EAPI virtual std::shared_ptr<IRenderer> SetViewport (unsigned int x, unsigned int y, unsigned int sx, unsigned int sy) override;
    
    __EAPI virtual void RenderFrame () override;
    
    __EAPI virtual void Draw (std::shared_ptr<MeshObject> meshObject) override;
    __EAPI virtual void DrawSurface () override;
    __EAPI virtual void DrawSceneGeometryBuffers (std::shared_ptr<IShaderProgram> shader) override;
    __EAPI virtual void DrawAABBGeometryBuffers (std::shared_ptr<IShaderProgram> shader) override;
    
    __EAPI virtual void Update (std::shared_ptr<MeshObject> meshObject) override;
    __EAPI virtual void UpdateAABBBuffers (std::shared_ptr<MeshObject> meshObject) override;
    __EAPI virtual AABB CalculateAABB (std::shared_ptr<MeshObject> meshObject) override;

    __EAPI virtual void Update (std::shared_ptr<Material> material, unsigned int textureUnit) override;
    __EAPI virtual void Update (std::shared_ptr<Material> material) override;
    
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
    void InitializeShaderLibrary ();
    
    void InitializeMatrixUniformBuffers ();
    void LoadMatrixUniformBuffers (std::shared_ptr<Camera> camera);
    void DeinitializeMatrixUniformBuffers ();    
    
    void InitializeLightViewMatrixUniformBuffers ();
    void LoadLightViewMatrixUniformBuffers ();
    void DeinitializeLightViewMatrixUniformBuffers ();

    void InitializeObjectBuffers ();
    void DeinitializeObjectBuffers ();

    void InitializeQuadGeometryBuffer ();
    void DeinitializeQuadGeometryBuffer ();
    
    void InitializeLightUniformBuffers ();
    void DeinitializeLightUniformBuffers ();
    void UpdateLightBufferRecursive (handle_t objectHandle);

    void RenderGeometryBuffer (SGlGeometryBuffers* buffer, GLuint type); 

private:
    // buffers with geometry data to be passed to GPU (key is object handle)
    TObjectGeometryBufferMap m_sceneGeometryBuffers;
    TObjectGeometryBufferMap m_aabbGeometryBuffers;
    SGlGeometryBuffers* m_surfaceGeometryBuffer;

    // Buffers for uniforms shared by entire scene
    SGlUniformBuffers* m_uniformBuffers;

    // data structures for uniforms
    SGlUniformMatrixBuffer* m_uniformMatrixBuffer;
    SGlUniformLightViewMatrixBuffer* m_uniformLightViewMatrixBuffer;
    SGlUniformPointLightBuffer* m_uniformPointLightBuffer;
    SGlUniformDirectionalLightBuffer* m_uniformDirectionalLightBuffer;
    SGlUniformSpotLightBuffer* m_uniformSpotLightBuffer;

    // materials texture units (key is material handle)
    TMaterialTextureUnitsMap m_materialTextureUnits;

    // maps gameobject handle to index in 
    // uniformPointLightBuffer
    // uniformDirectionalLightBuffer
    // uniformSpotLightBuffer
    TLightHandleIdxMap m_pointLights;
    TLightHandleIdxMap m_directionalLights;
    TLightHandleIdxMap m_spotLights;

};

} // namespace cilantro
