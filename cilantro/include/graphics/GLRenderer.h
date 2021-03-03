#ifndef _GLRENDERER_H_
#define _GLRENDERER_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "graphics/GLFramebuffer.h"
#if (CILANTRO_GL_VERSION > 140)
#include "graphics/GLMultisampleFramebuffer.h"
#endif
#include "graphics/GLShader.h"
#include "graphics/GLShaderProgram.h"
#include "graphics/Renderer.h"
#include "graphics/RenderTarget.h"
#include <cstring>

#define MAX_POINT_LIGHTS 64
#define MAX_DIRECTIONAL_LIGHTS 64
#define MAX_SPOT_LIGHTS 64
#define MAX_TEXTURE_UNITS (GL_TEXTURE15 - GL_TEXTURE0)

enum VBOType { VBO_VERTICES = 0, VBO_NORMALS, VBO_UVS };
enum UBOType { UBO_MATRICES = 0, UBO_POINTLIGHTS, UBO_DIRECTIONALLIGHTS, UBO_SPOTLIGHTS };
enum BindingPoint { BP_MATRICES = 1, BP_POINTLIGHTS, BP_DIRECTIONALLIGHTS, BP_SPOTLIGHTS };

struct ObjectBuffers
{
public:
    // Vertex Buffer Objects (vertices, normals, uvs)
    GLuint VBO[3];
    // Element Buffer Object (face indices)
    GLuint EBO;
    // Vertex Array Object
    GLuint VAO;
};

struct MaterialTextureUnits
{
public:
    // how many units in use 
    unsigned int unitsCount;
    // using 16 texture units, as per minimum defined in OpenGL 3.x
    GLuint textureUniforms[MAX_TEXTURE_UNITS];
    GLuint textureUnits[MAX_TEXTURE_UNITS];
};

struct SceneBuffers
{
public:
    // Uniform Buffer Objects (view & projection matrices, point lights, directional lights, spot lights)
    GLuint UBO[4];
};

struct UniformMatrixBuffer
{
public:
    // view matrix
    GLfloat viewMatrix[16];
    // projection matrix
    GLfloat projectionMatrix[16];
};

struct PointLightStruct
{
public:
    GLfloat lightPosition[3];
    GLfloat pad1;
    GLfloat lightColor[3];
    GLfloat attenuationConst;
    GLfloat attenuationLinear;
    GLfloat attenuationQuadratic;
};

struct DirectionalLightStruct
{
public:
    GLfloat lightDirection[3];
    GLfloat pad1;
    GLfloat lightColor[3];
};

struct SpotLightStruct
{
public:
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

struct UniformPointLightBuffer
{
public:
    // number of active point lights
    GLint pointLightCount;
    // pad to std140 specification
    GLint pad[3];
    // array of active point lights
    PointLightStruct pointLights[MAX_POINT_LIGHTS];
};

struct UniformDirectionalLightBuffer
{
public:
    // number of active directional lights
    GLint directionalLightCount;
    // pad to std140 specification
    GLint pad[3];
    // array of active point lights
    DirectionalLightStruct directionalLights[MAX_DIRECTIONAL_LIGHTS];
};

struct UniformSpotLightBuffer
{
public:
    // number of active spot lights
    GLint spotLightCount;
    // pad to std140 specification
    GLint pad[3];
    // array of active point lights
    SpotLightStruct spotLights[MAX_SPOT_LIGHTS];
};

class GLRenderer : public Renderer
{
public:
    __EAPI GLRenderer (unsigned int width, unsigned int height);
    __EAPI ~GLRenderer ();

    // (de)initializers
    __EAPI void Initialize ();
    __EAPI void Deinitialize ();

    // render
    __EAPI void RenderFrame ();

    // renderbuffer
    __EAPI void SetResolution (unsigned int width, unsigned int height);
    __EAPI GLuint GetRendererFramebuffer () const;
    __EAPI GLuint GetRendererFramebufferTexture () const;

    // object drawing and updating
    __EAPI void Draw (MeshObject& meshObject);
    __EAPI void Update (MeshObject& meshObject);
    __EAPI void Update (PointLight& pointLight);
    __EAPI void Update (DirectionalLight& directionalLight);	
    __EAPI void Update (SpotLight& spotLight);
    __EAPI void Update (Material& material, unsigned int textureUnit = 0);

private:

#if (CILANTRO_GL_VERSION <= 140)
    GLFramebuffer* framebuffer;
#else
    GLMultisampleFramebuffer* framebuffer;
#endif

    // GL buffers and arrays for scene objects
    // These buffers contain data for objects to be rendered
    // * MeshObject vertices, uvs, normals, etc
    // key is object handle
    std::unordered_map <unsigned int, ObjectBuffers> objectBuffers;

    // materials texture units
    // key is material handle
    std::unordered_map <unsigned int, MaterialTextureUnits> materialTextureUnits;

    // map of channel numbers to GL codes
    static std::unordered_map<unsigned int, GLint> textureChannelMap;

    // GL buffers for uniforms shared by entire scene:
    // * view and projection matrix
    // * array of lights
    SceneBuffers sceneBuffers;

    // data structures for uniforms
    UniformMatrixBuffer uniformMatrixBuffer;
    UniformPointLightBuffer uniformPointLightBuffer;
    UniformDirectionalLightBuffer uniformDirectionalLightBuffer;
    UniformSpotLightBuffer uniformSpotLightBuffer;

    // maps gameobject handle to index in 
    // uniformPointLightBuffer
    // uniformDirectionalLightBuffer
    // uniformSpotLightBuffer
    std::unordered_map<unsigned int, unsigned int> pointLights;
    std::unordered_map<unsigned int, unsigned int> directionalLights;
    std::unordered_map<unsigned int, unsigned int> spotLights;

    // check for GL errors
    void CheckGLError (const std::string& functionName);

    // initialize shader library
    void InitializeShaderLibrary ();
    // initialize object buffers
    void InitializeObjectBuffers ();
    // initialize object texture units
    void InitializeMaterialTextures ();
    // initialize uniform buffers of view and projection matrices
    void InitializeMatrixUniformBuffers ();
    // initialize uniform buffers of lights
    void InitializeLightUniformBuffers ();

    // recursively reload buffers of lights
    void UpdateLightBufferRecursive (unsigned int objectHandle);

    // update uniform buffers of view and projection matrices
    void LoadMatrixUniformBuffers ();

};

#endif
