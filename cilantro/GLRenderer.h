#ifndef _GLRENDERER_H_
#define _GLRENDERER_H_

#include "cilantroengine.h"
#include "GL/gl3w.h"
#include "GLShader.h"
#include "GLShaderModel.h"
#include "Renderer.h"
#include "GameScene.h"
#include "RenderTarget.h"
#include <cstring>

#define MAX_POINT_LIGHTS 64
#define MAX_DIRECTIONAL_LIGHTS 16

enum VBOType { VBO_VERTICES = 0, VBO_NORMALS, VBO_UVS };
enum UBOType { UBO_MATRICES = 0, UBO_POINTLIGHTS, UBO_DIRECTIONALLIGHTS };
enum BindingPoint { BP_MATRICES = 1, BP_POINTLIGHTS, BP_DIRECTIONALLIGHTS };

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
	// Uniform Buffer Objects (view & projection matrices, point lights, directional lights)
	GLuint UBO[3];
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

class GLRenderer : public Renderer
{
public:
	__EAPI GLRenderer (GameScene& scene, RenderTarget& target);
	__EAPI ~GLRenderer ();

	// initialize renderer
	__EAPI void Initialize ();
	// render one frame
	__EAPI void RenderFrame ();
	// deinitialize renderer
	__EAPI void Deinitialize ();

	// shader library manipulation
	__EAPI virtual void AddShader (std::string shaderName, std::string shaderSourceCode, ShaderType shaderType);
	__EAPI virtual void AddShaderToModel (std::string shaderModelName, std::string shaderName);
	__EAPI GLShaderModel& GetShaderModel (std::string shaderModelName);

	// object drawing and updating
	__EAPI void Draw (MeshObject& meshObject);
	__EAPI void Update (MeshObject& meshObject);
	__EAPI void Update (PointLight& pointLight);
	__EAPI void Update (DirectionalLight& directionalLight);	

private:

	// GL shader library
	std::unordered_map <std::string, GLShader> shaders;
	std::unordered_map <std::string, GLShaderModel> shaderModels;

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

    // maps gameobject handle to index in uniformPointLightBuffer
	std::unordered_map<unsigned int, unsigned int> pointLights;
	// maps gameobject handle to index in uniformDirectionalLightBuffer
	std::unordered_map<unsigned int, unsigned int> directionalLights;

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
