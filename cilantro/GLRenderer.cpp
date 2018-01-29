#include "GLRenderer.h"

#include "default.vs.h"
#include "phong.fs.h"
#include "blinnphong.fs.h"
#include "normals.fs.h"

#include <imgui.h>
#include "examples/opengl3_example/imgui_impl_glfw_gl3.h"

GLRenderer::GLRenderer (int xRes, int yRes) : xResolution (xRes), yResolution (yRes)
{
	glfwInit ();
}

GLRenderer::~GLRenderer ()
{
	glfwTerminate ();
}

void GLRenderer::Initialize (GameScene* scene)
{
	// initialize superclass
	Renderer::Initialize (scene);

	// set up GL & window properties
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint (GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint (GLFW_SAMPLES, 4);

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

	// enable face culling
	glFrontFace (GL_CCW);
	glEnable (GL_CULL_FACE);

	// enable antialiasing
	glEnable (GL_MULTISAMPLE);

	// initialize GLEW
	if (glewInit () != GLEW_OK)
	{
		LogMessage (__FUNCTION__, EXIT_FAILURE) << "GLEW initialization failed";
	}

	// Setup ImGui binding
	ImGui_ImplGlfwGL3_Init (window, true);
	ImGui::StyleColorsClassic ();

	// initialize shader library
	InitializeShaderLibrary ();

	// initialize object buffers
	InitializeObjectBuffers ();

	// initialize uniform buffers of view and projection matrices
	InitializeMatrixUniformBuffers ();

	// initialize uniform buffers of lights
	InitializeLightBuffers ();

	// set callback for new MeshObjects
	renderedScene->RegisterCallback ("OnUpdateMeshObject", [ & ](unsigned int objectHandle) { LoadObjectBuffers (objectHandle); });

	// set callback for new or modified PointLights
	renderedScene->RegisterCallback ("OnUpdatePointLight", [ & ](unsigned int objectHandle) { LoadLightBuffers (objectHandle); });

	// set callback for modified scene graph (currently this only requires to reload light buffers)
	renderedScene->RegisterCallback ("OnUpdateSceneGraph", [ & ](unsigned int objectHandle) { LoadLightUniformBuffers (); });

	// set callback for modified transforms (currently this only requires to reload light buffers)
	renderedScene->RegisterCallback ("OnUpdateTransform", [ & ](unsigned int objectHandle) { LoadLightUniformBuffers (); });

	// check for any outstanding errors
	CheckGLError (__FUNCTION__);
}

void GLRenderer::RenderFrame ()
{
	// invoke base class function
	Renderer::RenderFrame ();

	// tick imgui
	ImGui_ImplGlfwGL3_NewFrame ();

	// load uniform buffers
	LoadMatrixUniformBuffers ();

	// draw all objects scene
	for (GameObject* gameObject : renderedScene->GetGameObjects ())
	{
		gameObject->OnDraw (*this);
	}

	// update game clocks (Tock)
	Time::Tock ();

	// display debug messages
	if (Time::GetTimeSinceSplitTime () > 1.0f)
	{
		timeSinceLastSplit = Time::GetTimeSinceSplitTime ();
		Time::ResetSplitTime ();
		splitFrameCount = GetFrameCount () - lastFrameCount;
		lastFrameCount = GetFrameCount ();
		frameRenderTimeInLastSplit = frameRenderTimeSinceLastSplit;
		frameRenderTimeSinceLastSplit = 0;
	}
	frameRenderTimeSinceLastSplit += Time::GetFrameRenderTime ();
	ImGui::Text ("FPS: %.1f", splitFrameCount / timeSinceLastSplit);
	ImGui::Text ("Frame time: %.1f ms", frameRenderTimeInLastSplit / splitFrameCount * 1000.0f);
	
	// render imgui
	ImGui::Render ();

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
	pointLights.clear ();
	shaders.clear ();
	shaderModels.clear ();
	ImGui_ImplGlfwGL3_Shutdown ();
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
		if (searchModel == shaderModels.end ())
		{
			LogMessage (__FUNCTION__) << "Registered shader" << shaderModelName << "with id" << shaderModels[shaderModelName].GetProgramId ();
		}
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
	//TODO: get uniform location queries to be moved to initialization and invoked one time
	GLuint ambientColorId;
	GLuint diffuseColorId;
	GLuint specularColorId;
	GLuint emissiveColorId;
	GLuint specularShininessId;
	GLuint eyePositionId;
	GLuint modelMatrixId;
	GLuint normalMatrixId;
	GLuint shaderProgramId;
	
	// pick shader
	GLShaderModel& shaderProgram = GetShaderModel (meshObject.GetMaterial ().GetShaderModelName ());
	shaderProgramId = shaderProgram.GetProgramId ();
	shaderProgram.Use ();

	// get material properties for drawn objects and set uniform value
	ambientColorId = glGetUniformLocation (shaderProgramId, "fAmbientColor");
	glUniform3fv (ambientColorId, 1, &meshObject.GetMaterial ().GetAmbientColor ()[0]);
	
	diffuseColorId = glGetUniformLocation (shaderProgramId, "fDiffuseColor");
	glUniform3fv (diffuseColorId, 1, &meshObject.GetMaterial ().GetDiffuseColor ()[0]);

	emissiveColorId = glGetUniformLocation (shaderProgramId, "fEmissiveColor");
	glUniform3fv (emissiveColorId, 1, &meshObject.GetMaterial ().GetEmissiveColor ()[0]);

	specularColorId = glGetUniformLocation (shaderProgramId, "fSpecularColor");
	glUniform3fv (specularColorId, 1, &meshObject.GetMaterial ().GetSpecularColor ()[0]);
	specularShininessId = glGetUniformLocation (shaderProgramId, "fSpecularShininess");
	glUniform1f (specularShininessId, meshObject.GetMaterial ().GetSpecularShininess ());

	// get camera position in world space and set uniform value
	eyePositionId = glGetUniformLocation (shaderProgramId, "eyePosition");
	glUniform3fv (eyePositionId, 1, &renderedScene->GetActiveCamera ()->GetPosition ()[0]);

	// get world matrix for drawn objects and set uniform value
	modelMatrixId = glGetUniformLocation (shaderProgramId, "mModel");
	glUniformMatrix4fv (modelMatrixId, 1, GL_TRUE, meshObject.GetModelTransformMatrix ()[0]);

	// calculate normal matrix for drawn objects and set uniform value
	normalMatrixId = glGetUniformLocation (shaderProgramId, "mNormal");
	glUniformMatrix3fv (normalMatrixId, 1, GL_TRUE, Mathf::Invert (Mathf::Transpose (Matrix3f (meshObject.GetModelTransformMatrix ())))[0]);

	// draw mesh
	glBindVertexArray (objectBuffers[meshObject.GetHandle ()].VAO);
	glDrawElements (GL_TRIANGLES, meshObject.GetFaceCount () * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray (0);
}

void GLRenderer::CheckGLError (std::string functionName)
{
	GLuint errorCode;

	if ((errorCode = glGetError ()) != GL_NO_ERROR)
	{
		LogMessage (functionName, EXIT_FAILURE) << "glError:" << std::hex << std::showbase << errorCode;
	}
}

void GLRenderer::InitializeShaderLibrary ()
{
	// initialize shader library
	AddShader ("default_vertex_shader", gDefaultVertexShader, ShaderType::VERTEX_SHADER);
	AddShader ("phong_fragment_shader", gPhongFragmentShader, ShaderType::FRAGMENT_SHADER);
	AddShader ("blinnphong_fragment_shader", gBlinnPhongFragmentShader, ShaderType::FRAGMENT_SHADER);
	AddShader ("normals_fragment_shader", gNormalsFragmentShader, ShaderType::FRAGMENT_SHADER);

	// Phong model
	AddShaderToModel ("phong_shader", "default_vertex_shader");
	AddShaderToModel ("phong_shader", "phong_fragment_shader");

	// Blinn-Phong model
	AddShaderToModel ("blinnphong_shader", "default_vertex_shader");
	AddShaderToModel ("blinnphong_shader", "blinnphong_fragment_shader");

	// Normals visualization model
	AddShaderToModel ("normals_shader", "default_vertex_shader");
	AddShaderToModel ("normals_shader", "normals_fragment_shader");
}

void GLRenderer::InitializeObjectBuffers ()
{
	// load object buffers for all existing objects
	for (GameObject* gameObject : renderedScene->GetGameObjects ())
	{
		unsigned int objectHandle = gameObject->GetHandle ();
		// load buffers for MeshObject only
		if (dynamic_cast<MeshObject*>(gameObject) != nullptr)
		{
			LoadObjectBuffers (objectHandle);
		}
	}
}

void GLRenderer::InitializeMatrixUniformBuffers ()
{
	GLuint shaderProgramId;
	GLuint uniformMatricesBlockIndex;

	// create and pre-load uniform buffer for view and projection matrices
	glGenBuffers (1, &sceneBuffers.UBO[UBO_MATRICES]);
	glBindBuffer (GL_UNIFORM_BUFFER, sceneBuffers.UBO[UBO_MATRICES]);
	glBufferData (GL_UNIFORM_BUFFER, sizeof (UniformMatrixBuffer), &uniformMatrixBuffer, GL_DYNAMIC_DRAW);
	glBindBufferBase (GL_UNIFORM_BUFFER, BindingPoint::BP_MATRICES, sceneBuffers.UBO[UBO_MATRICES]);

	// set uniform block bindings
	for (auto &shaderModel : shaderModels)
	{
		shaderProgramId = shaderModel.second.GetProgramId ();
		uniformMatricesBlockIndex = glGetUniformBlockIndex (shaderProgramId, "UniformMatricesBlock");
		glUniformBlockBinding (shaderProgramId, uniformMatricesBlockIndex, BindingPoint::BP_MATRICES);
	}
}

void GLRenderer::InitializeLightBuffers ()
{
	GLuint shaderProgramId;
	GLuint uniformPointLightsBlockIndex;

	// initialize struct
	uniformPointLightBuffer.pointLightCount = 0;

	// create uniform buffer for point lights
	glGenBuffers (1, &sceneBuffers.UBO[UBO_POINTLIGHTS]);
	glBindBuffer (GL_UNIFORM_BUFFER, sceneBuffers.UBO[UBO_POINTLIGHTS]);
	glBufferData (GL_UNIFORM_BUFFER, sizeof (UniformPointLightBuffer), &uniformPointLightBuffer, GL_STATIC_DRAW);
	glBindBufferBase (GL_UNIFORM_BUFFER, BindingPoint::BP_POINTLIGHTS, sceneBuffers.UBO[UBO_POINTLIGHTS]);

	// set uniform block bindings in shaders
	for (auto &shaderModel : shaderModels)
	{
		shaderProgramId = shaderModel.second.GetProgramId ();
		uniformPointLightsBlockIndex = glGetUniformBlockIndex (shaderProgramId, "UniformPointLightsBlock");
		if (uniformPointLightsBlockIndex == GL_INVALID_INDEX)
		{
			LogMessage (__FUNCTION__, EXIT_FAILURE) << "Unable to locate uniform block";
		}
		glUniformBlockBinding (shaderProgramId, uniformPointLightsBlockIndex, BindingPoint::BP_POINTLIGHTS);
	}

	// scan objects vector for point lights and populate point lights collection
	for (GameObject* gameObject : renderedScene->GetGameObjects ())
	{
		unsigned int objectHandle = gameObject->GetHandle ();
		// populate collection for PointLight only
		if (dynamic_cast<PointLight*>(gameObject) != nullptr)
		{
			LoadLightBuffers (objectHandle);
		}
	}

}

void GLRenderer::LoadObjectBuffers (unsigned int objectHandle)
{
	MeshObject* myMeshObject = dynamic_cast<MeshObject*>(renderedScene->GetGameObjects ()[objectHandle]);

	// check of object's buffers are already initialized
	auto find = objectBuffers.find (objectHandle);

	if (find == objectBuffers.end ())
	{
		// it is a new object, perform full buffers initialization and load data

		objectBuffers.insert ({ objectHandle, ObjectBuffers () });

		LogMessage (__FUNCTION__) << "New MeshObject" << objectHandle;

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
		LogMessage (__FUNCTION__) << "Modified MeshObject" << objectHandle;

		// bind Vertex Array Object (VAO)
		glBindVertexArray (objectBuffers[objectHandle].VAO);

		// load vertex buffer
		glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_VERTICES]);
		glBufferData (GL_ARRAY_BUFFER, myMeshObject->GetVertexCount () * sizeof (float), myMeshObject->GetVerticesData (), GL_STATIC_DRAW);

		// load normals buffer
		glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_NORMALS]);
		glBufferData (GL_ARRAY_BUFFER, myMeshObject->GetVertexCount () * sizeof (float), myMeshObject->GetNormalsData (), GL_STATIC_DRAW);

		// load index buffer
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, objectBuffers[objectHandle].EBO);
		glBufferData (GL_ELEMENT_ARRAY_BUFFER, myMeshObject->GetFaceCount () * sizeof (unsigned int), myMeshObject->GetFacesData (), GL_STATIC_DRAW);

		// unbind VAO
		glBindVertexArray (0);

	}
}

void GLRenderer::LoadMatrixUniformBuffers ()
{
	Camera* activeCamera;

	// get active camera of rendered scene
	activeCamera = renderedScene->GetActiveCamera ();

	if (activeCamera == nullptr) 
	{
		LogMessage (__FUNCTION__, EXIT_FAILURE) << "No active camera found";
	}

	// load view matrix
	std::memcpy (uniformMatrixBuffer.viewMatrix, Mathf::Transpose (activeCamera->GetViewMatrix ())[0], 16 * sizeof (GLfloat));

	// load projection matrix
	std::memcpy (uniformMatrixBuffer.projectionMatrix, Mathf::Transpose (activeCamera->GetProjectionMatrix (xResolution, yResolution))[0], 16 * sizeof (GLfloat));

	// load to GPU
	glBindBuffer (GL_UNIFORM_BUFFER, sceneBuffers.UBO[UBO_MATRICES]);
	glBufferSubData (GL_UNIFORM_BUFFER, 0, 2 * 16 * sizeof (GLfloat), &uniformMatrixBuffer);
	glBindBuffer (GL_UNIFORM_BUFFER, 0);

}

void GLRenderer::LoadLightBuffers (unsigned int objectHandle)
{
	PointLight* myPointLightObject = dynamic_cast<PointLight*>(renderedScene->GetGameObjects ()[objectHandle]);

	// check if light is already in collection
	auto find = pointLights.find (objectHandle);

	if (find == pointLights.end ()) 
	{
		LogMessage (__FUNCTION__) << "New PointLight" << objectHandle;
		pointLights.insert ({ objectHandle, myPointLightObject->IsEnabled () });
	}
	else 
	{
		LogMessage (__FUNCTION__) << "Modified PointLight" << objectHandle;
		pointLights[objectHandle] = myPointLightObject->IsEnabled ();
	}

	// load uniform buffers
	LoadLightUniformBuffers ();
}

void GLRenderer::LoadLightUniformBuffers ()
{
	unsigned int lightId = 0;
	PointLight* myPointLightObject;

	// clear buffer
	uniformPointLightBuffer.pointLightCount = 0;
	
	// copy collection to light struct
	for (auto pointLight : pointLights) 
	{
		lightId = uniformPointLightBuffer.pointLightCount;
		myPointLightObject = dynamic_cast<PointLight*>(renderedScene->GetGameObjects ()[pointLight.first]);

		// copy only active lights
		if (pointLight.second == true)
		{

			uniformPointLightBuffer.pointLightCount++;

			// copy position
			Vector4f lightPosition = myPointLightObject->GetPosition ();
			uniformPointLightBuffer.pointLights[lightId].lightPosition[0] = lightPosition[0];
			uniformPointLightBuffer.pointLights[lightId].lightPosition[1] = lightPosition[1];
			uniformPointLightBuffer.pointLights[lightId].lightPosition[2] = lightPosition[2];

			// copy ambience and specular powers
			uniformPointLightBuffer.pointLights[lightId].ambiencePower = myPointLightObject->GetAmbiencePower ();
			uniformPointLightBuffer.pointLights[lightId].specularPower = myPointLightObject->GetSpecularPower ();

			// copy attenuation factors
			uniformPointLightBuffer.pointLights[lightId].attenuationConst = myPointLightObject->GetConstantAttenuationFactor ();
			uniformPointLightBuffer.pointLights[lightId].attenuationLinear = myPointLightObject->GetLinearAttenuationFactor ();
			uniformPointLightBuffer.pointLights[lightId].attenuationQuadratic = myPointLightObject->GetQuadraticAttenuationFactor ();

			// copy color
			uniformPointLightBuffer.pointLights[lightId].lightColor[0] = myPointLightObject->GetColor ()[0];
			uniformPointLightBuffer.pointLights[lightId].lightColor[1] = myPointLightObject->GetColor ()[1];
			uniformPointLightBuffer.pointLights[lightId].lightColor[2] = myPointLightObject->GetColor ()[2];

		}

	}

	// load uniform buffer for point lights
	glBindBuffer (GL_UNIFORM_BUFFER, sceneBuffers.UBO[UBO_POINTLIGHTS]);
	glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (UniformPointLightBuffer), &uniformPointLightBuffer);
	glBindBuffer (GL_UNIFORM_BUFFER, 0);

}


