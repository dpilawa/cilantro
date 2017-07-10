#ifndef _GLRENDERER_H_
#define _GLRENDERER_H_

#include <unordered_map>
#include "Matrix3f.h"
#include "Time.h"
#include "Mathf.h"
#include "Renderer.h"
#include "GameScene.h"
#include "LogMessage.h"
#include "MeshObject.h"
#include "GLShader.h"
#include "GLShaderModel.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

enum VBOType { VBO_VERTICES = 0, VBO_NORMALS, VBO_UVS };
enum BindingPoint { BP_UNIFORMS = 1 };

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
	// Uniform Buffer Object (V&P matrices, lights)
	GLuint UBO;
};

struct UniformBufferMatrices
{
public:
	// view matrix
	GLfloat ViewMatrix[16];
	// projection matrix
	GLfloat ProjectionMatrix[16];
};

class GLRenderer : public Renderer
{
public:
	GLRenderer (GameScene& scene,  int xRes, int yRes);
	~GLRenderer ();

	// initialize renderer
	void Initialize ();
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

	// data structure for uniforms
	UniformBufferMatrices uniformBufferMatrices;

	// initialize object buffers
	void InitializeObjectBuffers ();

	// initialize uniform buffers
	void InitializeUniformBuffers ();

	// re(load) object buffers of mesh objects
	void LoadObjectBuffers (unsigned int objectHandle);

	// reload uniform buffers of mesh objects
	void LoadUniformBuffers ();

};

#endif