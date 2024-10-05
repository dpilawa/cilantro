#pragma once

#include "cilantroengine.h"
#include "glad/glad.h"
#include "graphics/Renderer.h"

class CGameScene;
class MeshObject;
class Camera;

enum EGlVboType { VBO_VERTICES = 0, VBO_NORMALS, VBO_UVS, VBO_TANGENTS, VBO_BITANGENTS, VBO_BONES, VBO_BONEWEIGHTS };
enum EGlUboType { UBO_MATRICES = 0, UBO_POINTLIGHTS, UBO_DIRECTIONALLIGHTS, UBO_SPOTLIGHTS, UBO_DIRECTIONALLIGHTVIEWMATRICES };

struct SGlGeometryBuffers;
struct SGlMaterialTextureUnits;

typedef std::unordered_map <handle_t, SGlGeometryBuffers*> TObjectGeometryBufferMap;
typedef std::unordered_map <unsigned int, SGlMaterialTextureUnits*> TMaterialTextureUnitsMap;
typedef std::unordered_map<unsigned int, unsigned int> TLightHandleIdxMap;

struct SGlGeometryBuffers
{
    // number of vertices
    unsigned int indexCount;
    // Vertex Buffer Objects (vertices, normals, uvs, tangents, bitangents, bone indices, bone weights)
    GLuint VBO[CILANTRO_VBO_COUNT];
    // Element Buffer Object (face indices)
    GLuint EBO;
    // Vertex Array Object
    GLuint VAO;
};

struct SGlUniformBuffers
{
    // Uniform Buffer Objects (view & projection matrices, point lights, directional lights, spot lights, directional light view transforms)
    GLuint UBO[CILANTRO_UBO_COUNT];
};

struct SGlUniformMatrixBuffer
{
    // view matrix
    GLfloat viewMatrix[16];
    // projection matrix
    GLfloat projectionMatrix[16];
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

class CGLRenderer : public CRenderer
{
public:
    CGLRenderer (CGameScene* gameScene, unsigned int width, unsigned int height, bool shadowMappingEnabled, bool deferredRenderingEnabled);
    virtual ~CGLRenderer ();

    ///////////////////////////////////////////////////////////////////////////

    virtual void Initialize () override;
    virtual void Deinitialize () override;

    virtual IRenderer& SetViewport (unsigned int x, unsigned int y, unsigned int sx, unsigned int sy) override;

    virtual void RenderFrame () override;

    virtual void Draw (MeshObject& meshObject) override;
    virtual void DrawQuad () override;
    virtual void DrawAllGeometryBuffers (IShaderProgram& shader) override;

    virtual void Update (MeshObject& meshObject) override;
    virtual void Update (Material& material, unsigned int textureUnit) override;
    virtual void Update (Material& material) override;
    
    virtual void Update (PointLight& pointLight) override;
    virtual void Update (DirectionalLight& directionalLight) override;	
    virtual void Update (SpotLight& spotLight) override;

    virtual void UpdateCameraBuffers (Camera& camera) override;
    virtual void UpdateLightViewBuffers () override;

    virtual size_t GetPointLightCount () const override;
    virtual size_t GetDirectionalLightCount () const override;
    virtual size_t GetSpotLightCount () const override;

    virtual IFramebuffer* CreateFramebuffer (size_t width, size_t height, size_t rgbTextureCount, size_t rgbaTextureCount, size_t depthBufferArrayTextureCount, bool depthStencilRenderbufferEnabled, bool multisampleEnabled) override;
    virtual void BindDefaultFramebuffer () override;
    virtual void BindDefaultDepthBuffer () override;
    virtual void BindDefaultStencilBuffer () override;
    virtual void BindDefaultTextures () override;    

    virtual void ClearColorBuffer (const Vector4f& rgba) override;
    virtual void ClearDepthBuffer () override;
    virtual void ClearStencilBuffer () override;

    virtual void SetDepthTestEnabled (bool value) override;
    virtual void SetFaceCullingEnabled (bool value) override;
    virtual void SetMultisamplingEnabled (bool value) override;
    
    virtual void SetStencilTestEnabled (bool value) override;
    virtual void SetStencilTestFunction (EStencilTestFunction testFunction, int testValue) override;
    virtual void SetStencilTestOperation (EStencilTestOperation sFail, EStencilTestOperation dpFail, EStencilTestOperation dpPass) override;

    ///////////////////////////////////////////////////////////////////////////

private:
    void InitializeShaderLibrary ();
    
    void InitializeMatrixUniformBuffers ();
    void LoadViewProjectionUniformBuffers (Camera* camera);
    void LoadLightViewUniformBuffers ();
    void DeinitializeMatrixUniformBuffers ();    
    
    void InitializeObjectBuffers ();
    void DeinitializeObjectBuffers ();

    void InitializeQuadGeometryBuffer ();
    void DeinitializeQuadGeometryBuffer ();
    
    void InitializeLightUniformBuffers ();
    void DeinitializeLightUniformBuffers ();
    void UpdateLightBufferRecursive (unsigned int objectHandle);

    void RenderGeometryBuffer (SGlGeometryBuffers* buffer); 
  
    void CheckGLError (const std::string& location);

private:
    // buffers with geometry data to be passed to GPU (key is object handle)
    TObjectGeometryBufferMap m_sceneGeometryBuffers;
    SGlGeometryBuffers* m_quadGeometryBuffer;

    // Buffers for uniforms shared by entire scene
    SGlUniformBuffers* m_uniformBuffers;

    // data structures for uniforms
    SGlUniformMatrixBuffer* m_uniformMatrixBuffer;
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
