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
	GLuint shaderProgramId;
	GLuint uniformBlockIndex;

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

	// enable depth test
	glEnable (GL_DEPTH_TEST);


	// initialize GLEW
	if (glewInit () != GLEW_OK)
	{
		LogMessage (__FUNCTION__, EXIT_FAILURE) << "GLEW initialization failed";
	}

	// initialize object buffers
	InitializeObjectBuffers ();

	// initialize uniform buffers
	InitializeUniformBuffers ();

	// initialize shader library
	AddShader ("default_vertex_shader", gDefaultVertexShader, ShaderType::VERTEX_SHADER);
	AddShader ("default_fragment_shader", gDefaultFragmentShader, ShaderType::FRAGMENT_SHADER);
	AddShaderToModel ("default_shader", "default_vertex_shader");
	AddShaderToModel ("default_shader", "default_fragment_shader");
	
	// set uniform block binding
	shaderProgramId = shaderModels["default_shader"].GetProgramId ();
	uniformBlockIndex = glGetUniformBlockIndex (shaderProgramId, "UniformBlock");
	glUniformBlockBinding (shaderProgramId, uniformBlockIndex, BindingPoint::BP_UNIFORMS);

	// set callback for new MeshObjects
	renderedScene.RegisterCallback ("OnUpdateMeshObject", [&] (unsigned int objectHandle) { LoadObjectBuffers (objectHandle); });
}

void GLRenderer::RenderFrame ()
{
	// invoke base class function
	Renderer::RenderFrame ();

	// load uniform buffers
	LoadUniformBuffers ();

	// draw all objects scene
	for (GameObject* gameObject : renderedScene.GetGameObjects ())
	{
		gameObject->OnDraw (*this);
	}

	// update game clocks (Tock)
	Time::Tock ();

	// swap front and back buffers
	glfwSwapBuffers (window);

	// clear frame and depth buffers
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// poll input
	glfwPollEvents ();
}

void GLRenderer::Deinitialize ()
{
	objectBuffers.clear ();
	shaders.clear ();
	shaderModels.clear ();
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
	GLuint modelMatrixId;
	GLuint normalMatrixId;
	GLuint shaderProgramId;

	// draw mesh
	// TODO: this to be retrieved from Material class
	GLShaderModel& shaderProgram = GetShaderModel ("default_shader");
	shaderProgramId = shaderProgram.GetProgramId ();

	// get world matrix for drawn objects and set uniform value
	modelMatrixId = glGetUniformLocation (shaderProgramId, "mModel");
	glUniformMatrix4fv (modelMatrixId, 1, GL_TRUE, meshObject.GetModelTransformMatrix ().getDataPointer ());

	// calculate normal matrix for drawn objects and set uniform value
	normalMatrixId = glGetUniformLocation (shaderProgramId, "mNormal");
	glUniformMatrix3fv (normalMatrixId, 1, GL_TRUE, Inverse (Transpose (Matrix3f (meshObject.GetModelTransformMatrix ()))).getDataPointer ());

	// draw
	shaderProgram.Use ();
	glBindVertexArray (objectBuffers[meshObject.GetHandle ()].VAO);
	glDrawElements (GL_TRIANGLES, meshObject.GetFaceCount () * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray (0);
}

void GLRenderer::InitializeObjectBuffers ()
{
	for (GameObject* gameObject : renderedScene.GetGameObjects ())
	{
		unsigned int objectHandle = gameObject->GetHandle ();
		LoadObjectBuffers (objectHandle);
	}
}

void GLRenderer::InitializeUniformBuffers ()
{
	// create and pre-load uniform buffer
	glGenBuffers (1, &sceneBuffers.UBO);
	glBindBuffer (GL_UNIFORM_BUFFER, sceneBuffers.UBO);
	glBufferData (GL_UNIFORM_BUFFER, sizeof (uniformBufferMatrices), &uniformBufferMatrices, GL_DYNAMIC_DRAW);
	glBindBufferBase (GL_UNIFORM_BUFFER, BindingPoint::BP_UNIFORMS, sceneBuffers.UBO);
}

void GLRenderer::LoadObjectBuffers (unsigned int objectHandle)
{
	MeshObject* myMeshObject = dynamic_cast<MeshObject*>(renderedScene.GetGameObjects ()[objectHandle]);

	// check of object's buffers are already initialized
	auto find = objectBuffers.find (objectHandle);

	if (find == objectBuffers.end ())
	{
		// it is a new object, perform full buffers initialization and load data

		objectBuffers.insert ({ objectHandle, ObjectBuffers () });

		LogMessage (__FUNCTION__) << "NEW" << objectHandle;

		// generate and bind Vertex Array Object (VAO)
		glGenVertexArrays (1, &objectBuffers[objectHandle].VAO);
		glBindVertexArray (objectBuffers[objectHandle].VAO);

		// generate vertex buffer and copy vertices to GPU
		glGenBuffers (1, &objectBuffers[objectHandle].VBO[VBOType::VBO_VERTICES]);
		glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_VERTICES]);
		glBufferData (GL_ARRAY_BUFFER, myMeshObject->GetVertexCount () * sizeof (float), myMeshObject->GetVerticesData (), GL_STATIC_DRAW);
		// location = 0 (vertex position)
		glVertexAttribPointer (VBOType::VBO_VERTICES, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void*)0);

		// generate normals buffer and copy normals to GPU
		glGenBuffers (1, &objectBuffers[objectHandle].VBO[VBOType::VBO_NORMALS]);
		glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_NORMALS]);
		glBufferData (GL_ARRAY_BUFFER, myMeshObject->GetVertexCount () * sizeof (float), myMeshObject->GetNormalsData (), GL_STATIC_DRAW);
		// location = 1 (vertex normal)
		glVertexAttribPointer (VBOType::VBO_NORMALS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void*)0);

		// generate index buffer and copy face indices to GPU
		glGenBuffers (1, &objectBuffers[objectHandle].EBO);
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, objectBuffers[objectHandle].EBO);
		glBufferData (GL_ELEMENT_ARRAY_BUFFER, myMeshObject->GetFaceCount () * sizeof (unsigned int), myMeshObject->GetFacesData (), GL_STATIC_DRAW);

		// enable VBO arrays
		glEnableVertexAttribArray (VBOType::VBO_VERTICES);
		glEnableVertexAttribArray (VBOType::VBO_NORMALS);

		// unbind VAO
		glBindVertexArray (0);
	}
	else
	{
		// it is an existing object which has been modified
		LogMessage (__FUNCTION__) << "MODIFIED" << objectHandle;

		// bind Vertex Array Object (VAO)
		glBindVertexArray (objectBuffers[objectHandle].VAO);

		// load vertex buffer
		glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_VERTICES]);
		glBufferSubData (GL_ARRAY_BUFFER, 0, myMeshObject->GetVertexCount () * sizeof (float), myMeshObject->GetVerticesData ());

		// load normals buffer
		glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_NORMALS]);
		glBufferSubData (GL_ARRAY_BUFFER, 0, myMeshObject->GetVertexCount () * sizeof (float), myMeshObject->GetNormalsData ());

		// load index buffer
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, objectBuffers[objectHandle].EBO);
		glBufferSubData (GL_ELEMENT_ARRAY_BUFFER, 0, myMeshObject->GetFaceCount () * sizeof (unsigned int), myMeshObject->GetFacesData ());

		// unbind VAO
		glBindVertexArray (0);

	}
}

void GLRenderer::LoadUniformBuffers ()
{

	// load view matrix
	// TODO: this matrix should be a property of camera
	std::memcpy (uniformBufferMatrices.ViewMatrix, Transpose (Mathf::GenCameraViewMatrix (Vector3f (1.0f, 2.0f, 5.0f), Vector3f (0.0f, -1.0f, 0.0f), Vector3f (0.0f, 1.0f, 0.0f))).getDataPointer (), 16 * sizeof (GLfloat));

	// load projection matrix
	// TODO: this matrix should be a property of camera
	std::memcpy (uniformBufferMatrices.ProjectionMatrix, Transpose (Mathf::GenPerspectiveProjectionMatrix (4.0f / 3.0f, Mathf::Deg2Rad (75.0f), 0.1f, 100.0f)).getDataPointer (), 16 * sizeof (GLfloat));

	// load to GPU
	glBindBuffer (GL_UNIFORM_BUFFER, sceneBuffers.UBO);
	glBufferSubData (GL_UNIFORM_BUFFER, 0, 2 * 16 * sizeof (GLfloat), &uniformBufferMatrices);
	glBindBuffer (GL_UNIFORM_BUFFER, 0);

}


