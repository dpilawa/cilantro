#include "cilantroengine.h"
#include "Renderer.h"
#include "GLRenderer.h"
#include "GLShader.h"
#include "GLShaderModel.h"
#include "GameScene.h"
#include "GameObject.h"
#include "MeshObject.h"
#include "Light.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "Camera.h"
#include "RenderTarget.h"
#include "CallbackProvider.h"
#include "Mathf.h"
#include "LogMessage.h"
#include "Timer.h"
#include <unordered_map>
#include <cmath>

#include "default.vs.h"
#include "phong.fs.h"
#include "blinnphong.fs.h"
#include "normals.fs.h"

#include "imgui.h"

GLRenderer::GLRenderer () : Renderer ()
{
}

GLRenderer::~GLRenderer ()
{
}

void GLRenderer::Initialize ()
{
	// enable depth test
	glEnable (GL_DEPTH_TEST);

	// enable face culling
	glFrontFace (GL_CCW);
	glEnable (GL_CULL_FACE);

	// enable antialiasing
	glEnable (GL_MULTISAMPLE);

	// initialize shader library
	InitializeShaderLibrary ();

	// initialize object buffers
	InitializeObjectBuffers ();

	// initialize uniform buffers of view and projection matrices
	InitializeMatrixUniformBuffers ();

	// initialize uniform buffers of lights
	InitializeLightUniformBuffers ();

	// initialize framebuffer

	// set callback for new MeshObjects
    gameLoop->GetScene ().RegisterCallback ("OnUpdateMeshObject", [&](unsigned int objectHandle) { gameLoop->GetScene ().GetGameObjects ()[objectHandle]->OnUpdate (*this); });

    // set callback for new or modified PointLights
    gameLoop->GetScene ().RegisterCallback ("OnUpdateLight", [&](unsigned int objectHandle) { gameLoop->GetScene ().GetGameObjects ()[objectHandle]->OnUpdate (*this); });

    // set callback for modified scene graph (currently this only requires to reload light buffers)
    gameLoop->GetScene ().RegisterCallback ("OnUpdateSceneGraph", [&](unsigned int objectHandle) { UpdateLightBufferRecursive (objectHandle); });

    // set callback for modified transforms (currently this only requires to reload light buffers)
    gameLoop->GetScene ().RegisterCallback ("OnUpdateTransform", [&](unsigned int objectHandle) { UpdateLightBufferRecursive (objectHandle); });

    // check for any outstanding errors
	CheckGLError (__func__);

	LogMessage (__func__) << "GLRenderer started";
}

void GLRenderer::RenderFrame ()
{
	// invoke rendering target's pre-frame function
	gameLoop->GetRenderTarget().BeforeFrame ();

	// clear frame and depth buffers
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// load uniform buffers
	LoadMatrixUniformBuffers ();

	// bind render target
    gameLoop->GetRenderTarget ().Bind ();

    // draw all objects scene
	for (auto gameObject : gameLoop->GetScene ().GetGameObjects ())
	{
		gameObject.second->OnDraw (*this);
	}

	// update game clocks (Tock)
	Timer::Tock ();

	// invoke rendering target's post-frame function
    gameLoop->GetRenderTarget ().AfterFrame ();
}

void GLRenderer::Deinitialize ()
{
	objectBuffers.clear ();
	pointLights.clear ();
	shaders.clear ();
	shaderModels.clear ();
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
		LogMessage (__func__, EXIT_FAILURE) << "Shader " << shaderName << " not found when adding to model " << shaderModelName;
	}
	else
	{
		shaderModels[shaderModelName].AttachShader (searchShader->second);
		if (searchModel == shaderModels.end ())
		{
			LogMessage (__func__) << "Registered shader" << shaderModelName << "with id" << shaderModels[shaderModelName].GetProgramId ();
		}
	}
}

GLShaderModel & GLRenderer::GetShaderModel (std::string shaderModelName)
{
	auto searchModel = shaderModels.find (shaderModelName);

	if (searchModel == shaderModels.end ())
	{
		LogMessage (__func__, EXIT_FAILURE) << "Unable to find shader model " << shaderModelName;
	}

	return searchModel->second;
}

void GLRenderer::Draw (MeshObject & meshObject)
{
	//TODO: get uniform location queries to be moved to initialization and called one time
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
	if (ambientColorId != GL_INVALID_INDEX)
	{
		glUniform3fv (ambientColorId, 1, &meshObject.GetMaterial ().GetAmbientColor ()[0]);
	}

	diffuseColorId = glGetUniformLocation (shaderProgramId, "fDiffuseColor");
	if (diffuseColorId != GL_INVALID_INDEX)
	{
		glUniform3fv (diffuseColorId, 1, &meshObject.GetMaterial ().GetDiffuseColor ()[0]);
	}

	emissiveColorId = glGetUniformLocation (shaderProgramId, "fEmissiveColor");
	if (emissiveColorId != GL_INVALID_INDEX)
	{
		glUniform3fv (emissiveColorId, 1, &meshObject.GetMaterial ().GetEmissiveColor ()[0]);
	}

	specularColorId = glGetUniformLocation (shaderProgramId, "fSpecularColor");
	if (specularColorId != GL_INVALID_INDEX)
	{
		glUniform3fv (specularColorId, 1, &meshObject.GetMaterial ().GetSpecularColor ()[0]);
	}

	specularShininessId = glGetUniformLocation (shaderProgramId, "fSpecularShininess");
	if (specularShininessId != GL_INVALID_INDEX)
	{
		glUniform1f (specularShininessId, meshObject.GetMaterial ().GetSpecularShininess ());
	}

	// get camera position in world space and set uniform value
	eyePositionId = glGetUniformLocation (shaderProgramId, "eyePosition");
	if (eyePositionId != GL_INVALID_INDEX)
	{
		glUniform3fv (eyePositionId, 1, &gameLoop->GetScene ().GetActiveCamera ()->GetPosition ()[0]);
	}

	// get world matrix for drawn objects and set uniform value
	modelMatrixId = glGetUniformLocation (shaderProgramId, "mModel");
	if (modelMatrixId != GL_INVALID_INDEX)
	{
		glUniformMatrix4fv (modelMatrixId, 1, GL_TRUE, meshObject.GetModelTransformMatrix ()[0]);
	}

	// calculate normal matrix for drawn objects and set uniform value
	normalMatrixId = glGetUniformLocation (shaderProgramId, "mNormal");
	if (normalMatrixId != GL_INVALID_INDEX)
	{
		glUniformMatrix3fv (normalMatrixId, 1, GL_TRUE, Mathf::Invert (Mathf::Transpose (Matrix3f (meshObject.GetModelTransformMatrix ())))[0]);
	}

	// draw mesh
	glBindVertexArray (objectBuffers[meshObject.GetHandle ()].VAO);
	glDrawElements (GL_TRIANGLES, meshObject.GetFaceCount () * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray (0);
}

void GLRenderer::Update (MeshObject& meshObject)
{
    unsigned int objectHandle = meshObject.GetHandle ();

    // check of object's buffers are already initialized
	auto find = objectBuffers.find (objectHandle);

	if (find == objectBuffers.end ())
	{
		// it is a new object, perform full buffers initialization and load data

		objectBuffers.insert ({ objectHandle, ObjectBuffers () });

		LogMessage (__func__) << "New MeshObject" << objectHandle;

		// generate and bind Vertex Array Object (VAO)
		glGenVertexArrays (1, &objectBuffers[objectHandle].VAO);
		glBindVertexArray (objectBuffers[objectHandle].VAO);

		// generate vertex buffer and copy vertices to GPU
		glGenBuffers (1, &objectBuffers[objectHandle].VBO[VBOType::VBO_VERTICES]);
		glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_VERTICES]);
		glBufferData (GL_ARRAY_BUFFER, meshObject.GetVertexCount () * sizeof (float), meshObject.GetVerticesData (), GL_STATIC_DRAW);
		// location = 0 (vertex position)
		glVertexAttribPointer (VBOType::VBO_VERTICES, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void*)0);

		// generate normals buffer and copy normals to GPU
		glGenBuffers (1, &objectBuffers[objectHandle].VBO[VBOType::VBO_NORMALS]);
		glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_NORMALS]);
		glBufferData (GL_ARRAY_BUFFER, meshObject.GetVertexCount () * sizeof (float), meshObject.GetNormalsData (), GL_STATIC_DRAW);
		// location = 1 (vertex normal)
		glVertexAttribPointer (VBOType::VBO_NORMALS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void*)0);

		// generate index buffer and copy face indices to GPU
		glGenBuffers (1, &objectBuffers[objectHandle].EBO);
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, objectBuffers[objectHandle].EBO);
		glBufferData (GL_ELEMENT_ARRAY_BUFFER, meshObject.GetFaceCount () * sizeof (unsigned int), meshObject.GetFacesData (), GL_STATIC_DRAW);

		// enable VBO arrays
		glEnableVertexAttribArray (VBOType::VBO_VERTICES);
		glEnableVertexAttribArray (VBOType::VBO_NORMALS);

		// unbind VAO
		glBindVertexArray (0);
	}
	else
	{
		// it is an existing object which has been modified

		// bind Vertex Array Object (VAO)
		glBindVertexArray (objectBuffers[objectHandle].VAO);

		// load vertex buffer
		glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_VERTICES]);
		glBufferData (GL_ARRAY_BUFFER, meshObject.GetVertexCount () * sizeof (float), meshObject.GetVerticesData (), GL_STATIC_DRAW);

		// load normals buffer
		glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_NORMALS]);
		glBufferData (GL_ARRAY_BUFFER, meshObject.GetVertexCount () * sizeof (float), meshObject.GetNormalsData (), GL_STATIC_DRAW);

		// load index buffer
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, objectBuffers[objectHandle].EBO);
		glBufferData (GL_ELEMENT_ARRAY_BUFFER, meshObject.GetFaceCount () * sizeof (unsigned int), meshObject.GetFacesData (), GL_STATIC_DRAW);

		// unbind VAO
		glBindVertexArray (0);

	}
}

void GLRenderer::Update (PointLight& pointLight)
{
    unsigned int objectHandle = pointLight.GetHandle ();
    unsigned int lightId;
    unsigned int uniformBufferOffset;

    // check if light is already in collection
	auto find = pointLights.find (objectHandle);

    if (find == pointLights.end ())
	{
		LogMessage (__func__) << "New PointLight" << objectHandle;
		lightId = uniformPointLightBuffer.pointLightCount++;
		pointLights.insert ({ objectHandle, lightId });
	}
	else
	{
		// existing light modified
		lightId = pointLights[objectHandle];
	}

	// copy position
	Vector4f lightPosition = pointLight.GetPosition ();
	uniformPointLightBuffer.pointLights[lightId].lightPosition[0] = lightPosition[0];
	uniformPointLightBuffer.pointLights[lightId].lightPosition[1] = lightPosition[1];
	uniformPointLightBuffer.pointLights[lightId].lightPosition[2] = lightPosition[2];

	// copy ambience and specular powers
	uniformPointLightBuffer.pointLights[lightId].ambiencePower = pointLight.GetAmbiencePower ();
	uniformPointLightBuffer.pointLights[lightId].specularPower = pointLight.GetSpecularPower ();

	// copy attenuation factors
	uniformPointLightBuffer.pointLights[lightId].attenuationConst = pointLight.GetConstantAttenuationFactor ();
	uniformPointLightBuffer.pointLights[lightId].attenuationLinear = pointLight.GetLinearAttenuationFactor ();
	uniformPointLightBuffer.pointLights[lightId].attenuationQuadratic = pointLight.GetQuadraticAttenuationFactor ();

	// copy color
	uniformPointLightBuffer.pointLights[lightId].lightColor[0] = pointLight.GetColor ()[0];
	uniformPointLightBuffer.pointLights[lightId].lightColor[1] = pointLight.GetColor ()[1];
	uniformPointLightBuffer.pointLights[lightId].lightColor[2] = pointLight.GetColor ()[2];

	// copy to GPU memory
	glBindBuffer (GL_UNIFORM_BUFFER, sceneBuffers.UBO[UBO_POINTLIGHTS]);

	// load light counts
	glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (uniformPointLightBuffer.pointLightCount), &uniformPointLightBuffer.pointLightCount);

	// load uniform buffer for a light at given index
	uniformBufferOffset = sizeof (uniformPointLightBuffer.pointLightCount) + 3 * sizeof (GLint) + lightId * sizeof (PointLightStruct);
	glBufferSubData (GL_UNIFORM_BUFFER, uniformBufferOffset, sizeof (PointLightStruct), &uniformPointLightBuffer.pointLights[lightId]);

	glBindBuffer (GL_UNIFORM_BUFFER, 0);

}

void GLRenderer::Update (DirectionalLight& directionalLight)
{
    unsigned int objectHandle = directionalLight.GetHandle ();
    unsigned int lightId;
    unsigned int uniformBufferOffset;

    // check if light is already in collection
	auto find = directionalLights.find (objectHandle);

    if (find == directionalLights.end ())
	{
		LogMessage (__func__) << "New DirectionalLight" << objectHandle;
		lightId = uniformDirectionalLightBuffer.directionalLightCount++;
		directionalLights.insert ({ objectHandle, lightId });
	}
	else
	{
		// existing light modified
		lightId = directionalLights[objectHandle];
	}

	// copy direction
	Vector3f lightDirection = directionalLight.GetForward ();
	uniformDirectionalLightBuffer.directionalLights[lightId].lightDirection[0] = lightDirection[0];
	uniformDirectionalLightBuffer.directionalLights[lightId].lightDirection[1] = lightDirection[1];
	uniformDirectionalLightBuffer.directionalLights[lightId].lightDirection[2] = lightDirection[2];

	// copy ambience and specular powers
	uniformDirectionalLightBuffer.directionalLights[lightId].ambiencePower = directionalLight.GetAmbiencePower ();
	uniformDirectionalLightBuffer.directionalLights[lightId].specularPower = directionalLight.GetSpecularPower ();

	// copy color
	uniformDirectionalLightBuffer.directionalLights[lightId].lightColor[0] = directionalLight.GetColor ()[0];
	uniformDirectionalLightBuffer.directionalLights[lightId].lightColor[1] = directionalLight.GetColor ()[1];
	uniformDirectionalLightBuffer.directionalLights[lightId].lightColor[2] = directionalLight.GetColor ()[2];

	// copy to GPU memory
	glBindBuffer (GL_UNIFORM_BUFFER, sceneBuffers.UBO[UBO_DIRECTIONALLIGHTS]);

	// load light counts
	glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (uniformDirectionalLightBuffer.directionalLightCount), &uniformDirectionalLightBuffer.directionalLightCount);

	// load uniform buffer for a light at given index
	uniformBufferOffset = sizeof (uniformDirectionalLightBuffer.directionalLightCount) + 3 * sizeof (GLint) + lightId * sizeof (DirectionalLightStruct);
	glBufferSubData (GL_UNIFORM_BUFFER, uniformBufferOffset, sizeof (DirectionalLightStruct), &uniformDirectionalLightBuffer.directionalLights[lightId]);

	glBindBuffer (GL_UNIFORM_BUFFER, 0);
}

void GLRenderer::Update (SpotLight& spotLight)
{
    unsigned int objectHandle = spotLight.GetHandle ();
    unsigned int lightId;
    unsigned int uniformBufferOffset;

    // check if light is already in collection
	auto find = spotLights.find (objectHandle);

    if (find == spotLights.end ())
	{
		LogMessage (__func__) << "New SpotLight" << objectHandle;
		lightId = uniformSpotLightBuffer.spotLightCount++;
		spotLights.insert ({ objectHandle, lightId });
	}
	else
	{
		// existing light modified
		lightId = spotLights[objectHandle];
	}

	// copy position
	Vector4f lightPosition = spotLight.GetPosition ();
	uniformSpotLightBuffer.spotLights[lightId].lightPosition[0] = lightPosition[0];
	uniformSpotLightBuffer.spotLights[lightId].lightPosition[1] = lightPosition[1];
	uniformSpotLightBuffer.spotLights[lightId].lightPosition[2] = lightPosition[2];

	// copy direction
	Vector3f lightDirection = spotLight.GetForward ();
	uniformSpotLightBuffer.spotLights[lightId].lightDirection[0] = lightDirection[0];
	uniformSpotLightBuffer.spotLights[lightId].lightDirection[1] = lightDirection[1];
	uniformSpotLightBuffer.spotLights[lightId].lightDirection[2] = lightDirection[2];

	// copy ambience and specular powers
	uniformSpotLightBuffer.spotLights[lightId].ambiencePower = spotLight.GetAmbiencePower ();
	uniformSpotLightBuffer.spotLights[lightId].specularPower = spotLight.GetSpecularPower ();
	
	// copy attenuation factors
	uniformSpotLightBuffer.spotLights[lightId].attenuationConst = spotLight.GetConstantAttenuationFactor ();
	uniformSpotLightBuffer.spotLights[lightId].attenuationLinear = spotLight.GetLinearAttenuationFactor ();
	uniformSpotLightBuffer.spotLights[lightId].attenuationQuadratic = spotLight.GetQuadraticAttenuationFactor ();

	// copy cutoff angles
	uniformSpotLightBuffer.spotLights[lightId].innerCutoffCosine = std::cos (Mathf::Deg2Rad (spotLight.GetInnerCutoff ()));
	uniformSpotLightBuffer.spotLights[lightId].outerCutoffCosine = std::cos (Mathf::Deg2Rad (spotLight.GetOuterCutoff ()));

	// copy color
	uniformSpotLightBuffer.spotLights[lightId].lightColor[0] = spotLight.GetColor ()[0];
	uniformSpotLightBuffer.spotLights[lightId].lightColor[1] = spotLight.GetColor ()[1];
	uniformSpotLightBuffer.spotLights[lightId].lightColor[2] = spotLight.GetColor ()[2];

	// copy to GPU memory
	glBindBuffer (GL_UNIFORM_BUFFER, sceneBuffers.UBO[UBO_SPOTLIGHTS]);

	// load light counts
	glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (uniformSpotLightBuffer.spotLightCount), &uniformSpotLightBuffer.spotLightCount);

	// load uniform buffer for a light at given index
	uniformBufferOffset = sizeof (uniformSpotLightBuffer.spotLightCount) + 3 * sizeof (GLint) + lightId * sizeof (SpotLightStruct);
	glBufferSubData (GL_UNIFORM_BUFFER, uniformBufferOffset, sizeof (SpotLightStruct), &uniformSpotLightBuffer.spotLights[lightId]);

	glBindBuffer (GL_UNIFORM_BUFFER, 0);
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
	glBindAttribLocation(GetShaderModel("phong_shader").GetProgramId(), 0, "vPosition");
	glBindAttribLocation(GetShaderModel("phong_shader").GetProgramId(), 1, "vNormal");

	// Blinn-Phong model
	AddShaderToModel ("blinnphong_shader", "default_vertex_shader");
	AddShaderToModel ("blinnphong_shader", "blinnphong_fragment_shader");
	glBindAttribLocation(GetShaderModel("blinnphong_shader").GetProgramId(), 0, "vPosition");
	glBindAttribLocation(GetShaderModel("blinnphong_shader").GetProgramId(), 1, "vNormal");

	// Normals visualization model
	AddShaderToModel ("normals_shader", "default_vertex_shader");
	AddShaderToModel ("normals_shader", "normals_fragment_shader");
	glBindAttribLocation(GetShaderModel("normals_shader").GetProgramId(), 0, "vPosition");
	glBindAttribLocation(GetShaderModel("normals_shader").GetProgramId(), 1, "vNormal");
}

void GLRenderer::InitializeObjectBuffers ()
{
	// load object buffers for all existing objects
	for (auto gameObject : gameLoop->GetScene().GetGameObjects ())
	{
		// load buffers for MeshObject only
		if (dynamic_cast<MeshObject*>(gameObject.second) != nullptr)
		{
			gameObject.second->OnUpdate (*this);
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
	for (auto&& shaderModel : shaderModels)
	{
		shaderProgramId = shaderModel.second.GetProgramId ();
		uniformMatricesBlockIndex = glGetUniformBlockIndex (shaderProgramId, "UniformMatricesBlock");
		glUniformBlockBinding (shaderProgramId, uniformMatricesBlockIndex, BindingPoint::BP_MATRICES);
	}
}

void GLRenderer::InitializeLightUniformBuffers ()
{
	GLuint shaderProgramId;
	GLuint uniformPointLightsBlockIndex;
	GLuint uniformDirectionalLightsBlockIndex;
	GLuint uniformSpotLightsBlockIndex;

	// initialize structs
	uniformPointLightBuffer.pointLightCount = 0;
    uniformDirectionalLightBuffer.directionalLightCount = 0;
	uniformSpotLightBuffer.spotLightCount = 0;

    // create uniform buffer for point lights
	glGenBuffers (1, &sceneBuffers.UBO[UBO_POINTLIGHTS]);
	glBindBuffer (GL_UNIFORM_BUFFER, sceneBuffers.UBO[UBO_POINTLIGHTS]);
	glBufferData (GL_UNIFORM_BUFFER, sizeof (UniformPointLightBuffer), &uniformPointLightBuffer, GL_STATIC_DRAW);
	glBindBufferBase (GL_UNIFORM_BUFFER, BindingPoint::BP_POINTLIGHTS, sceneBuffers.UBO[UBO_POINTLIGHTS]);

    // create uniform buffer for directional lights
	glGenBuffers (1, &sceneBuffers.UBO[UBO_DIRECTIONALLIGHTS]);
	glBindBuffer (GL_UNIFORM_BUFFER, sceneBuffers.UBO[UBO_DIRECTIONALLIGHTS]);
	glBufferData (GL_UNIFORM_BUFFER, sizeof (UniformDirectionalLightBuffer), &uniformDirectionalLightBuffer, GL_STATIC_DRAW);
	glBindBufferBase (GL_UNIFORM_BUFFER, BindingPoint::BP_DIRECTIONALLIGHTS, sceneBuffers.UBO[UBO_DIRECTIONALLIGHTS]);

    // create uniform buffer for spot lights
	glGenBuffers (1, &sceneBuffers.UBO[UBO_SPOTLIGHTS]);
	glBindBuffer (GL_UNIFORM_BUFFER, sceneBuffers.UBO[UBO_SPOTLIGHTS]);
	glBufferData (GL_UNIFORM_BUFFER, sizeof (UniformSpotLightBuffer), &uniformSpotLightBuffer, GL_STATIC_DRAW);
	glBindBufferBase (GL_UNIFORM_BUFFER, BindingPoint::BP_SPOTLIGHTS, sceneBuffers.UBO[UBO_SPOTLIGHTS]);

	// set uniform block bindings in shaders which have it
	for (auto&& shaderModel : shaderModels)
	{
		shaderProgramId = shaderModel.second.GetProgramId ();
		
		uniformPointLightsBlockIndex = glGetUniformBlockIndex (shaderProgramId, "UniformPointLightsBlock");
		if (uniformPointLightsBlockIndex == GL_INVALID_INDEX)
		{
			LogMessage (__func__) << "Program id" << shaderModel.second.GetProgramId() << "has no UniformPointLightsBlock";
		}
		else {
			glUniformBlockBinding(shaderProgramId, uniformPointLightsBlockIndex, BindingPoint::BP_POINTLIGHTS);
		}

		uniformDirectionalLightsBlockIndex = glGetUniformBlockIndex (shaderProgramId, "UniformDirectionalLightsBlock");
		if (uniformDirectionalLightsBlockIndex == GL_INVALID_INDEX)
		{
			LogMessage (__func__) << "Program id" << shaderModel.second.GetProgramId() << "has no UniformDirectionalLightsBlock";
		}
		else {
			glUniformBlockBinding(shaderProgramId, uniformDirectionalLightsBlockIndex, BindingPoint::BP_DIRECTIONALLIGHTS);
		}

		uniformSpotLightsBlockIndex = glGetUniformBlockIndex (shaderProgramId, "UniformSpotLightsBlock");
		if (uniformSpotLightsBlockIndex == GL_INVALID_INDEX)
		{
			LogMessage (__func__) << "Program id" << shaderModel.second.GetProgramId() << "has no UniformSpotLightsBlock";
		}
		else {
			glUniformBlockBinding(shaderProgramId, uniformSpotLightsBlockIndex, BindingPoint::BP_SPOTLIGHTS);
		}

	}

	// scan objects vector for lights and populate light buffers
	for (auto gameObject : gameLoop->GetScene ().GetGameObjects ())
	{
		if (gameObject.second->IsLight ())
		{
            gameObject.second->OnUpdate (*this);
        }
    }

}

void GLRenderer::UpdateLightBufferRecursive (unsigned int objectHandle)
{
	GameObject* gameObject = gameLoop->GetScene ().GetGameObjects ()[objectHandle];

	if (gameObject->IsLight ())
	{
		gameObject->OnUpdate (*this);
	}

	for (auto&& childObject : gameObject->GetChildObjects ())
	{
		UpdateLightBufferRecursive (childObject->GetHandle ());
	}
}

void GLRenderer::LoadMatrixUniformBuffers ()
{
	Camera* activeCamera;

	// get active camera of rendered scene
	activeCamera = gameLoop->GetScene ().GetActiveCamera ();

	if (activeCamera == nullptr)
	{
		LogMessage (__func__, EXIT_FAILURE) << "No active camera found";
	}

	// load view matrix
	std::memcpy (uniformMatrixBuffer.viewMatrix, Mathf::Transpose (activeCamera->GetViewMatrix ())[0], 16 * sizeof (GLfloat));

	// load projection matrix
	std::memcpy (uniformMatrixBuffer.projectionMatrix, Mathf::Transpose (activeCamera->GetProjectionMatrix (gameLoop->GetRenderTarget ().GetXResolution (), gameLoop->GetRenderTarget ().GetYResolution ()))[0], 16 * sizeof (GLfloat));

	// load to GPU
	glBindBuffer (GL_UNIFORM_BUFFER, sceneBuffers.UBO[UBO_MATRICES]);
	glBufferSubData (GL_UNIFORM_BUFFER, 0, 2 * 16 * sizeof (GLfloat), &uniformMatrixBuffer);
	glBindBuffer (GL_UNIFORM_BUFFER, 0);

}


