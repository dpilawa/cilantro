#ifndef _GLRENDERER_H_
#define _GLRENDERER_H_

#include "cilantroengine.h"
#include "Renderer.h"
#include "LogMessage.h"
#include "Time.h"
#include "Mathf.h"
#include "Matrix3f.h"
#include "Vector4f.h"
#include "PointLight.h"
#include "GLShader.h"
#include "GLShaderModel.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <string>

#define MAX_LIGHTS 100

enum VBOType { VBO_VERTICES = 0, VBO_NORMALS, VBO_UVS };
enum UBOType { UBO_MATRICES = 0, UBO_POINTLIGHTS };
enum BindingPoint { BP_MATRICES = 1, BP_POINTLIGHTS };

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
	// Uniform Buffer Objects (view & projection matrices, lights)
	GLuint UBO[2];
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

struct UniformPointLightBuffer
{
public:
	// number of active point lights
	GLint pointLightCount;
	// pad to std140 specification
	GLint pad[3];
	// array of active point lights
	PointLightStruct pointLights[MAX_LIGHTS];
};

class GLRenderer : public Renderer
{
public:
	__EAPI GLRenderer (int xRes, int yRes);
	__EAPI ~GLRenderer ();

	// initialize renderer
	__EAPI void Initialize (GameScene* scene);
	// render one frame
	__EAPI void RenderFrame ();
	// deinitialize renderer
	__EAPI void Deinitialize ();

	// shader library manipulation
	__EAPI virtual void AddShader (std::string shaderName, std::string shaderSourceCode, ShaderType shaderType);
	__EAPI virtual void AddShaderToModel (std::string shaderModelName, std::string shaderName);
	__EAPI GLShaderModel& GetShaderModel (std::string shaderModelName);

	// object drawing functions
	__EAPI void Draw (MeshObject& meshObject);

private:
	// rendering resolution
	int xResolution;
	int yResolution;

	// GL window context
	GLFWwindow* window;

	// GL shader library
	std::unordered_map <std::string, GLShader> shaders;
	std::unordered_map <std::string, GLShaderModel> shaderModels;

	// GL buffers and arrays for scene objects
	// These buffers contain data for objects to be rendered
	// * MeshObject vertices, uvs, normals, etc
	std::unordered_map <unsigned int, ObjectBuffers> objectBuffers;

	// GL buffers for uniforms shared by entire scene:
	// * view and projection matrix
	// * vrray of lights
	SceneBuffers sceneBuffers;

	// data structures for uniforms
	UniformMatrixBuffer uniformMatrixBuffer;
	UniformPointLightBuffer uniformPointLightBuffer;

	// active lights collection
	std::unordered_map<unsigned int, bool> pointLights;

	// initialize shader library
	void InitializeShaderLibrary ();

	// initialize object buffers
	void InitializeObjectBuffers ();
	// initialize uniform buffers of view and projection matrices
	void InitializeMatrixUniformBuffers ();
	// initialize uniform buffers of lights
	void InitializeLightBuffers ();

	// re(load) object buffers of mesh objects
	void LoadObjectBuffers (unsigned int objectHandle);
	// reload uniform buffers of view and projection matrices
	void LoadMatrixUniformBuffers ();
	// reload buffers of lights
	void LoadLightBuffers (unsigned int objectHandle);
	void LoadLightUniformBuffers ();

};

#endif