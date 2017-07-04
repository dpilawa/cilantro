#ifndef _GLRENDERER_H_
#define _GLRENDERER_H_

#include <unordered_map>
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

struct Buffers
{
public:
	// Vertex Buffer Objects (vertices, normals, uvs)
	GLuint VBO[3];
	// Element Buffer Object
	GLuint EBO;
	// Vertex Array Object
	GLuint VAO;
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

	// GL buffers and arrays for all scene objects
	std::unordered_map <unsigned int, Buffers> buffers;

	// initialize object buffers
	void InitializeBuffers ();

	// re(load) object buffers of mesh objects
	void LoadBuffers (unsigned int objectHandle);

};

#endif