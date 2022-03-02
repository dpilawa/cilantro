#ifndef _GLRENDERER_H_
#define _GLRENDERER_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "graphics/Renderer.h"

class GameScene;
class MeshObject;

struct SGLGeometryBuffers : public SGeometryBuffers
{
    // Vertex Buffer Objects (vertices, normals, uvs, tangents, bitangents, bone indices, bone weights)
    GLuint VBO[CILANTRO_VBO_COUNT];
    // Element Buffer Object (face indices)
    GLuint EBO;
    // Vertex Array Object
    GLuint VAO;
};

struct SGLUniformBuffers : public SUniformBuffers
{
    // Uniform Buffer Objects (view & projection matrices, point lights, directional lights, spot lights)
    GLuint UBO[CILANTRO_UBO_COUNT];
};

struct SGLUniformMatrixBuffer : public SUniformMatrixBuffer
{
    // view matrix
    GLfloat viewMatrix[16];
    // projection matrix
    GLfloat projectionMatrix[16];
};

struct SGLMaterialTextureUnits : public SMaterialTextureUnits
{
    // using 16 texture units, as per minimum defined in OpenGL 3.x
    GLuint textureUnits[CILANTRO_MAX_TEXTURE_UNITS];
};

struct SGLPointLightStruct
{
    GLfloat lightPosition[3];
    GLfloat pad1;
    GLfloat lightColor[3];
    GLfloat attenuationConst;
    GLfloat attenuationLinear;
    GLfloat attenuationQuadratic;
};

struct SGLDirectionalLightStruct
{
    GLfloat lightDirection[3];
    GLfloat pad1;
    GLfloat lightColor[3];
};

struct SGLSpotLightStruct
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

struct SGLUniformPointLightBuffer : public SUniformPointLightBuffer
{
    // number of active point lights
    GLint pointLightCount;
    // pad to std140 specification
    GLint pad[3];
    // array of active point lights
    SGLPointLightStruct pointLights[CILANTRO_MAX_POINT_LIGHTS];
};

struct SGLUniformDirectionalLightBuffer : public SUniformDirectionalLightBuffer
{
    // number of active directional lights
    GLint directionalLightCount;
    // pad to std140 specification
    GLint pad[3];
    // array of active point lights
    SGLDirectionalLightStruct directionalLights[CILANTRO_MAX_DIRECTIONAL_LIGHTS];
};

struct SGLUniformSpotLightBuffer : public SUniformSpotLightBuffer
{
    // number of active spot lights
    GLint spotLightCount;
    // pad to std140 specification
    GLint pad[3];
    // array of active point lights
    SGLSpotLightStruct spotLights[CILANTRO_MAX_SPOT_LIGHTS];
};

class GLRenderer : public Renderer
{
public:
    __EAPI GLRenderer (GameScene* gameScene, unsigned int width, unsigned int height, bool isDeferred);
    __EAPI virtual ~GLRenderer ();

    __EAPI virtual void Initialize ();
    __EAPI virtual void Deinitialize ();
    __EAPI virtual void RenderFrame ();
    __EAPI virtual void RenderGeometryBuffer (SGeometryBuffers* buffer);    

    // framebuffer control
    virtual Framebuffer* CreateFramebuffer (unsigned int rgbTextures, unsigned int rgbaTextures, bool multisampleEnabled);
    virtual void BindDefaultFramebuffer ();

    virtual void ClearColorBuffer (const Vector4f& rgba);
    virtual void ClearDepthBuffer ();
    virtual void ClearStencilBuffer ();

    virtual void SetDepthTestEnabled (bool value);
    virtual void SetFaceCullingEnabled (bool value);
    virtual void SetMultisamplingEnabled (bool value);

    virtual void SetStencilTestEnabled (bool value);
    virtual void SetStencilTestFunction (StencilTestFunction testFunction, int testValue);

    // geometry
    virtual void Draw (MeshObject& meshObject) override;
    virtual void Update (MeshObject& meshObject) override;
    virtual void Update (Material& material, unsigned int textureUnit) override;
    virtual void Update (Material& material) override;
    virtual void Update (PointLight& pointLight) override;
    virtual void Update (DirectionalLight& directionalLight) override;
    virtual void Update (SpotLight& spotLight) override;

    virtual void LoadMatrixUniformBuffers () override;

private:

    void InitializeShaderLibrary ();

    void InitializeQuadGeometryBuffer ();
    void DeinitializeQuadGeometryBuffer ();

    virtual void InitializeMatrixUniformBuffers () override;

    // initialize uniform buffers of lights
    virtual void InitializeLightUniformBuffers () override;
    // recursively reload buffers of lights
    virtual void UpdateLightBufferRecursive (unsigned int objectHandle) override;

    GLuint GetTextureFormat (unsigned int numTextures);
    void CheckGLError (const std::string& location);

    // maps gameobject handle to index in 
    // uniformPointLightBuffer
    // uniformDirectionalLightBuffer
    // uniformSpotLightBuffer
    std::unordered_map<unsigned int, unsigned int> pointLights;
    std::unordered_map<unsigned int, unsigned int> directionalLights;
    std::unordered_map<unsigned int, unsigned int> spotLights;



};

#endif