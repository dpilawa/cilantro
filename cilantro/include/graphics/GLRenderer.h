#ifndef _GLRENDERER_H_
#define _GLRENDERER_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "graphics/GLMultisampleFramebuffer.h"
#include "graphics/GLShader.h"
#include "graphics/GLShaderProgram.h"
#include "graphics/Renderer.h"
#include "graphics/RenderTarget.h"
#include "scene/GameScene.h"
#include <cstring>

#define MAX_POINT_LIGHTS 64
#define MAX_DIRECTIONAL_LIGHTS 64
#define MAX_SPOT_LIGHTS 64

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
	GLfloat ambiencePower;
	GLfloat specularPower;
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
	GLfloat ambiencePower;
	GLfloat specularPower;
};

struct SpotLightStruct
{
public:
	GLfloat lightPosition[3];
	GLfloat pad1;
	GLfloat lightDirection[3];
	GLfloat pad2;
	GLfloat lightColor[3];
	GLfloat ambiencePower;
	GLfloat specularPower;
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

class GLRenderer : public Renderer, public GLMultisampleFramebuffer
{
public:
	GLRenderer () = delete;
	__EAPI GLRenderer (GameLoop* gameLoop, unsigned int width, unsigned int height);
	__EAPI ~GLRenderer ();

	// render
	__EAPI void RenderFrame ();

	// renderbuffer
    __EAPI void SetResolution (unsigned int width, unsigned int height);
    __EAPI GLuint GetRendererFramebuffer () const;
	__EAPI GLuint GetRendererFramebufferTexture () const;

    // shader library manipulation
	__EAPI virtual void AddShader (std::string shaderName, std::string shaderSourceCode, ShaderType shaderType);
	__EAPI virtual void AddShaderToProgram (std::string shaderProgramName, std::string shaderName);
	__EAPI GLShaderProgram& GetShaderProgram (std::string shaderProgramName);

	// object drawing and updating
	__EAPI void Draw (MeshObject& meshObject);
	__EAPI void Update (MeshObject& meshObject);
	__EAPI void Update (PointLight& pointLight);
	__EAPI void Update (DirectionalLight& directionalLight);	
	__EAPI void Update (SpotLight& spotLight);

private:

	// GL shader library
	std::unordered_map <std::string, GLShader> shaders;
	std::unordered_map <std::string, GLShaderProgram> shaderPrograms;

	// GL buffers and arrays for scene objects
	// These buffers contain data for objects to be rendered
	// * MeshObject vertices, uvs, normals, etc
	std::unordered_map <unsigned int, ObjectBuffers> objectBuffers;

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

	// (de)initializers
	void Initialize ();
	void Deinitialize ();

	// check for GL errors
	void CheckGLError (std::string functionName);

	// initialize shader library
	void InitializeShaderLibrary ();
	// initialize object buffers
	void InitializeObjectBuffers ();
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
