#ifndef _GLRENDERER_H_
#define _GLRENDERER_H_

#include <unordered_map>
#include <vector>
#include "Matrix3f.h"
#include "Time.h"
#include "Mathf.h"
#include "Renderer.h"
#include "GameScene.h"
#include "LogMessage.h"
#include "MeshObject.h"
#include "PointLight.h"
#include "GLShader.h"
#include "GLShaderModel.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

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
	GLfloat lightColor[3];
	GLfloat ambiencePower;
	GLfloat lightPosition[3];
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
	PointLightStruct pointLights[MAX_LIGHTS];
};

class GLRenderer : public Renderer
{
public:
	GLRenderer (int xRes, int yRes);
	~GLRenderer ();

	// initialize renderer
	void Initialize (GameScene* scene);
	// render one frame
	void RenderFrame ();
	// deinitialize renderer
	void Deinitialize ();

	// shader library manipulation
	virtual void AddShader (std::string shaderName, std::string shaderSourceCode, ShaderType shaderType);
	virtual void AddShaderToModel (std::string shaderModelName, std::string shaderName);
	GLShaderModel& GetShaderModel (std::string shaderModelName);

	// object drawing functions
	void Draw (MeshObject& meshObject);

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