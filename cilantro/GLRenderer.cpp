#include "GLRenderer.h"
#include "default.vs.h"
#include "default.fs.h"

GLRenderer::GLRenderer (GameScene& scene, int xRes, int yRes) : Renderer(scene), xResolution (xRes), yResolution (yRes)
{
	glfwInit ();
}


GLRenderer::~GLRenderer ()
{
	glfwTerminate ();
}

void GLRenderer::Initialize ()
{
	// set up GL & window properties
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint (GLFW_RESIZABLE, GL_FALSE);
	
	// create window
	window = glfwCreateWindow (xResolution, yResolution, "GL", nullptr, nullptr);
	if (window == NULL)
	{
		LogMessage (__FUNCTION__, EXIT_FAILURE) << "GLFW unable to create window";
	}

	// make openGL context active
	glfwMakeContextCurrent (window);

	// set vsync on
	glfwSwapInterval (1);

	// initialize GLEW
	if (glewInit () != GLEW_OK)
	{
		LogMessage (__FUNCTION__, EXIT_FAILURE) << "GLEW initialization failed";
	}

	// initialize shader library
	AddShader ("default_vertex_shader", gDefaultVertexShader, ShaderType::VERTEX_SHADER);
	AddShader ("default_fragment_shader", gDefaultFragmentShader, ShaderType::FRAGMENT_SHADER);
	AddShaderToModel ("default_shader", "default_vertex_shader");
	AddShaderToModel ("default_shader", "default_fragment_shader");

	// initialize object buffers
	InitializeBuffers ();

	// set callback for new MeshObjects
	renderedScene.RegisterCallback ("OnUpdateMeshObject", [&] (unsigned int objectHandle) { LoadBuffers (objectHandle); });
}

void GLRenderer::RenderFrame ()
{
	// invoke base class function
	Renderer::RenderFrame ();

	// draw all objects scene
	for (GameObject* gameObject : renderedScene.GetGameObjects ())
	{
		gameObject->OnDraw (*this);
	}

	// update game clocks (Tock)
	Time::Tock ();

	// swap front and back buffers
	glfwSwapBuffers (window);
	
	// poll input
	glfwPollEvents ();
}

void GLRenderer::Deinitialize ()
{
	buffers.clear ();
	glfwDestroyWindow (window);
}

void GLRenderer::AddShader (std::string shaderName, std::string shaderSourceCode, ShaderType shaderType)
{
	shaders[shaderName] = GLShader (shaderSourceCode, shaderType);
}

void GLRenderer::AddShaderToModel (std::string shaderModelName, std::string shaderName)
{
	auto searchModel = shaderModels.find (shaderModelName);
	auto searchShader = shaders.find (shaderName);

	if (searchShader == shaders.end ())
	{
		LogMessage (__FUNCTION__, EXIT_FAILURE) << "Shader " << shaderName << " not found when adding to model " << shaderModelName;
	}
	else
	{
		shaderModels[shaderModelName].AttachShader (searchShader->second);
	}
}

GLShaderModel & GLRenderer::GetShaderModel (std::string shaderModelName)
{
	auto searchModel = shaderModels.find (shaderModelName);

	if (searchModel == shaderModels.end ())
	{
		LogMessage (__FUNCTION__, EXIT_FAILURE) << "Unable to find shader model " << shaderModelName;
	}

	return searchModel->second;
}

void GLRenderer::Draw (MeshObject & meshObject)
{
	// draw mesh
	GetShaderModel("default_shader").Use ();
	glBindVertexArray (buffers[meshObject.GetHandle ()].VAO);
	glDrawElements (GL_TRIANGLES, meshObject.GetFaceCount () * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray (0);
}

void GLRenderer::InitializeBuffers ()
{
	for (GameObject* gameObject : renderedScene.GetGameObjects ())
	{
		unsigned int objectHandle = gameObject->GetHandle ();
		LoadBuffers (objectHandle);
	}
}

void GLRenderer::LoadBuffers (unsigned int objectHandle)
{
	buffers.insert_or_assign (objectHandle, Buffers ());
	MeshObject* myMeshObject = dynamic_cast<MeshObject*>(renderedScene.GetGameObjects ()[objectHandle]);

	LogMessage (__FUNCTION__) << objectHandle;

	// bind Vertex Array Object
	glGenVertexArrays (1, &buffers[objectHandle].VAO);
	glBindVertexArray (buffers[objectHandle].VAO);
		
	// copy vertices to GPU
	glGenBuffers (1, &buffers[objectHandle].VBO);
	glBindBuffer (GL_ARRAY_BUFFER, buffers[objectHandle].VBO);
	glBufferData (GL_ARRAY_BUFFER, myMeshObject->GetVertexCount () * sizeof (float), myMeshObject->GetVerticesData (), GL_STATIC_DRAW);

	// copy face indices to GPU
	glGenBuffers (1, &buffers[objectHandle].EBO);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, buffers[objectHandle].EBO);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, myMeshObject->GetFaceCount () * sizeof (unsigned int), myMeshObject->GetFacesData (), GL_STATIC_DRAW);

	// define vertex attributes
	// location = 0 (vertex position)
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void*)0);
	glEnableVertexAttribArray (0);
}


