#include "cilantroengine.h"
#include "graphics/Renderer.h"
#include "graphics/GLRenderer.h"
#include "graphics/GLShader.h"
#include "graphics/GLShaderProgram.h"
#include "graphics/GLPostprocess.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/MeshObject.h"
#include "scene/Light.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "scene/SpotLight.h"
#include "scene/Camera.h"
#include "util/CallbackProvider.h"
#include "util/LogMessage.h"
#include "math/Mathf.h"

#include <unordered_map>
#include <cmath>

#include "graphics/default.vs.h"
#include "graphics/flatquad.vs.h"
#include "graphics/phong.fs.h"
#include "graphics/blinnphong.fs.h"
#include "graphics/normals.fs.h"
#include "graphics/emissive.fs.h"
#include "graphics/flatquad.fs.h"
#include "graphics/post_gamma.fs.h"

GLRenderer::GLRenderer (GameLoop* gameLoop, unsigned int width, unsigned int height) : Renderer (gameLoop, width, height), 
#if (CILANTRO_MIN_GL_VERSION <= 140)
GLFramebuffer (width, height)
#else
GLMultisampleFramebuffer (width, height)
#endif
{
	this->Initialize ();
}

GLRenderer::~GLRenderer ()
{
	this->Deinitialize ();
}

void GLRenderer::RenderFrame ()
{
	// bind framebuffer
	BindFramebuffer ();

	// clear frame and depth buffers
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// enable depth test
	glEnable (GL_DEPTH_TEST);

	// load uniform buffers
	LoadMatrixUniformBuffers ();

	// set viewport
	glViewport (0, 0, this->width, this->height);

    // draw all objects scene
	for (auto gameObject : gameLoop->gameScene->GetGameObjects ())
	{
		gameObject.second->OnDraw (*this);
	}

#if (CILANTRO_MIN_GL_VERSION > 140)
	// blit framebuffer
	BlitFramebuffer ();
#endif

	// base class functions
	Renderer::RenderFrame ();
}

void GLRenderer::SetResolution (unsigned int width, unsigned int height)
{
	Renderer::SetResolution (width, height);

    this->SetFramebufferResolution (width, height);

	for (auto&& postprocess : postprocesses)
	{
        dynamic_cast<GLPostprocess*> (postprocess)->SetFramebufferResolution (width, height);
    }
}

GLuint GLRenderer::GetRendererFramebuffer () const
{
	if (postprocessStage==0) 
	{
        return this->GetFramebuffer ();
    }
    else 
	{
        return dynamic_cast<GLPostprocess*> (postprocesses[postprocessStage - 1])->GetFramebuffer ();
    }
}

GLuint GLRenderer::GetRendererFramebufferTexture () const
{
	if (postprocessStage==0) 
	{
        return this->GetFramebufferTexture ();
    }
    else 
	{
        return dynamic_cast<GLPostprocess*> (postprocesses[postprocessStage - 1])->GetFramebufferTexture ();
    }
}

void GLRenderer::AddShader (std::string shaderName, std::string shaderSourceCode, ShaderType shaderType)
{
	shaders[shaderName] = GLShader (shaderSourceCode, shaderType);
}

void GLRenderer::AddShaderToProgram (std::string shaderProgramName, std::string shaderName)
{
	auto searchProgram = shaderPrograms.find (shaderProgramName);
	auto searchShader = shaders.find (shaderName);

	if (searchShader == shaders.end ())
	{
		LogMessage (__func__, EXIT_FAILURE) << "Shader " << shaderName << " not found when adding to model " << shaderProgramName;
	}
	else
	{
		shaderPrograms[shaderProgramName].LinkShader (searchShader->second);
		if (searchProgram == shaderPrograms.end ())
		{
			LogMessage (__func__) << "Registered shader" << shaderProgramName << "with id" << shaderPrograms[shaderProgramName].GetProgramId ();
		}
	}
}

GLShaderProgram & GLRenderer::GetShaderProgram (std::string shaderProgramName)
{
	auto searchProgram = shaderPrograms.find (shaderProgramName);

	if (searchProgram == shaderPrograms.end ())
	{
		LogMessage (__func__, EXIT_FAILURE) << "Unable to find shader model " << shaderProgramName;
	}

	return searchProgram->second;
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

    GLuint drawMode;

    // pick shader
	GLShaderProgram& shaderProgram = GetShaderProgram (meshObject.GetMaterial ().GetShaderProgramName ());
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
		glUniform3fv (eyePositionId, 1, &gameLoop->gameScene->GetActiveCamera ()->GetPosition ()[0]);
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

	switch (meshObject.GetMeshType ())
	{
		case MeshType::Points:
            drawMode = GL_POINTS;
            break;

		case MeshType::Lines:
            drawMode = GL_LINES;
            break;

        case MeshType::Triangles:
            drawMode = GL_TRIANGLES;
            break;

        default:
            drawMode = GL_TRIANGLES;
            break;

    }

	glDrawElements (drawMode, meshObject.GetIndexCount (), GL_UNSIGNED_INT, nullptr);
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
		glBufferData (GL_ARRAY_BUFFER, meshObject.GetVertexCount () * sizeof (float) * 3, meshObject.GetVerticesData (), GL_STATIC_DRAW);
		// location = 0 (vertex position)
		glVertexAttribPointer (VBOType::VBO_VERTICES, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void*)0);

		// generate normals buffer and copy normals to GPU
		glGenBuffers (1, &objectBuffers[objectHandle].VBO[VBOType::VBO_NORMALS]);
		glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_NORMALS]);
		glBufferData (GL_ARRAY_BUFFER, meshObject.GetVertexCount () * sizeof (float) * 3, meshObject.GetNormalsData (), GL_STATIC_DRAW);
		// location = 1 (vertex normal)
		glVertexAttribPointer (VBOType::VBO_NORMALS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void*)0);

		// generate index buffer and copy face indices to GPU
		glGenBuffers (1, &objectBuffers[objectHandle].EBO);
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, objectBuffers[objectHandle].EBO);
		glBufferData (GL_ELEMENT_ARRAY_BUFFER, meshObject.GetIndexCount () * sizeof (unsigned int), meshObject.GetFacesData (), GL_STATIC_DRAW);

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
		glBufferData (GL_ARRAY_BUFFER, meshObject.GetVertexCount () * sizeof (float) * 3, meshObject.GetVerticesData (), GL_STATIC_DRAW);

		// load normals buffer
		glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_NORMALS]);
		glBufferData (GL_ARRAY_BUFFER, meshObject.GetVertexCount () * sizeof (float) * 3, meshObject.GetNormalsData (), GL_STATIC_DRAW);

		// load index buffer
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, objectBuffers[objectHandle].EBO);
		glBufferData (GL_ELEMENT_ARRAY_BUFFER, meshObject.GetIndexCount () * sizeof (unsigned int), meshObject.GetFacesData (), GL_STATIC_DRAW);

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

void GLRenderer::Initialize ()
{
    // display GL version information
	LogMessage (__func__) << "Version:" << (char*) glGetString (GL_VERSION);
    LogMessage (__func__) << "Shader language version:" << (char*) glGetString (GL_SHADING_LANGUAGE_VERSION);
	LogMessage (__func__) << "Renderer:" << (char*) glGetString (GL_RENDERER);

	// enable depth test
	glEnable (GL_DEPTH_TEST);

	// enable face culling
	glFrontFace (GL_CCW);
	glEnable (GL_CULL_FACE);

	// enable multisampling
	glEnable (GL_MULTISAMPLE);

	// initialize shader library
	InitializeShaderLibrary ();

	// initialize object buffers
	InitializeObjectBuffers ();

	// initialize uniform buffers of view and projection matrices
	InitializeMatrixUniformBuffers ();

	// initialize uniform buffers of lights
	InitializeLightUniformBuffers ();

	// set callback for new MeshObjects
    gameLoop->gameScene->RegisterCallback ("OnUpdateMeshObject", [&](unsigned int objectHandle) { gameLoop->gameScene->GetGameObjects ()[objectHandle]->OnUpdate (*this); });

    // set callback for new or modified PointLights
    gameLoop->gameScene->RegisterCallback ("OnUpdateLight", [&](unsigned int objectHandle) { gameLoop->gameScene->GetGameObjects ()[objectHandle]->OnUpdate (*this); });

    // set callback for modified scene graph (currently this only requires to reload light buffers)
    gameLoop->gameScene->RegisterCallback ("OnUpdateSceneGraph", [&](unsigned int objectHandle) { UpdateLightBufferRecursive (objectHandle); });

    // set callback for modified transforms (currently this only requires to reload light buffers)
    gameLoop->gameScene->RegisterCallback ("OnUpdateTransform", [&](unsigned int objectHandle) { UpdateLightBufferRecursive (objectHandle); });

    // check for any outstanding errors
	CheckGLError (__func__);

	LogMessage (__func__) << "GLRenderer started";
}

void GLRenderer::Deinitialize ()
{
	objectBuffers.clear ();
	pointLights.clear ();
	directionalLights.clear ();
	spotLights.clear ();
	shaders.clear ();
	shaderPrograms.clear ();
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
	AddShader ("flatquad_vertex_shader", gFlatQuadVertexShader, ShaderType::VERTEX_SHADER);
	AddShader ("phong_fragment_shader", gPhongFragmentShader, ShaderType::FRAGMENT_SHADER);
	AddShader ("blinnphong_fragment_shader", gBlinnPhongFragmentShader, ShaderType::FRAGMENT_SHADER);
	AddShader ("normals_fragment_shader", gNormalsFragmentShader, ShaderType::FRAGMENT_SHADER);
	AddShader ("emissive_fragment_shader", gEmissiveFragmentShader, ShaderType::FRAGMENT_SHADER);
	AddShader ("flatquad_fragment_shader", gFlatQuadFragmentShader, ShaderType::FRAGMENT_SHADER);
	AddShader ("post_gamma_fragment_shader", gPostGammaFragmentShader, ShaderType::FRAGMENT_SHADER);

    // Phong model
	AddShaderToProgram ("phong_shader", "default_vertex_shader");
	AddShaderToProgram ("phong_shader", "phong_fragment_shader");
#if (CILANTRO_MIN_GL_VERSION < 330)
	glBindAttribLocation(GetShaderProgram("phong_shader").GetProgramId(), 0, "vPosition");
	glBindAttribLocation(GetShaderProgram("phong_shader").GetProgramId(), 1, "vNormal");
#endif

	// Blinn-Phong model
	AddShaderToProgram ("blinnphong_shader", "default_vertex_shader");
	AddShaderToProgram ("blinnphong_shader", "blinnphong_fragment_shader");
#if (CILANTRO_MIN_GL_VERSION < 330)	
	glBindAttribLocation(GetShaderProgram("blinnphong_shader").GetProgramId(), 0, "vPosition");
	glBindAttribLocation(GetShaderProgram("blinnphong_shader").GetProgramId(), 1, "vNormal");
#endif

	// Normals visualization model
	AddShaderToProgram ("normals_shader", "default_vertex_shader");
	AddShaderToProgram ("normals_shader", "normals_fragment_shader");
#if (CILANTRO_MIN_GL_VERSION < 330)
	glBindAttribLocation(GetShaderProgram("normals_shader").GetProgramId(), 0, "vPosition");
	glBindAttribLocation(GetShaderProgram("normals_shader").GetProgramId(), 1, "vNormal");
#endif

	// Only emissive color rendering (no lights calculation)
	AddShaderToProgram ("emissive_shader", "default_vertex_shader");
	AddShaderToProgram ("emissive_shader", "emissive_fragment_shader");
#if (CILANTRO_MIN_GL_VERSION < 330)	
	glBindAttribLocation(GetShaderProgram("normals_shader").GetProgramId(), 0, "vPosition");
	glBindAttribLocation(GetShaderProgram("normals_shader").GetProgramId(), 1, "vNormal");
#endif

	// Screen quad rendering
	AddShaderToProgram ("flatquad_shader", "flatquad_vertex_shader");
	AddShaderToProgram ("flatquad_shader", "flatquad_fragment_shader");
#if (CILANTRO_MIN_GL_VERSION < 330)	
	glBindAttribLocation(GetShaderProgram("flatquad_shader").GetProgramId(), 0, "vPosition");
	glBindAttribLocation(GetShaderProgram("flatquad_shader").GetProgramId(), 1, "vTextureCoordinates");
#endif

	// Post-processing gamma
	AddShaderToProgram ("post_gamma_shader", "flatquad_vertex_shader");
	AddShaderToProgram ("post_gamma_shader", "post_gamma_fragment_shader");
#if (CILANTRO_MIN_GL_VERSION < 330)	
	glBindAttribLocation(GetShaderProgram("post_gamma_shader").GetProgramId(), 0, "vPosition");
	glBindAttribLocation(GetShaderProgram("post_gamma_shader").GetProgramId(), 1, "vTextureCoordinates");
#endif

}

void GLRenderer::InitializeObjectBuffers ()
{
	// load object buffers for all existing objects
	for (auto gameObject : gameLoop->gameScene->GetGameObjects ())
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
	for (auto&& shaderProgram : shaderPrograms)
	{
		shaderProgramId = shaderProgram.second.GetProgramId ();
		uniformMatricesBlockIndex = glGetUniformBlockIndex (shaderProgramId, "UniformMatricesBlock");
		if (uniformMatricesBlockIndex == GL_INVALID_INDEX)
		{
			LogMessage (__func__) << "Program id" << shaderProgram.second.GetProgramId() << "has no UniformMatricesBlock";
		}
		else {
			glUniformBlockBinding (shaderProgramId, uniformMatricesBlockIndex, BindingPoint::BP_MATRICES);
		}		
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
	for (auto&& shaderProgram : shaderPrograms)
	{
		shaderProgramId = shaderProgram.second.GetProgramId ();
		
		uniformPointLightsBlockIndex = glGetUniformBlockIndex (shaderProgramId, "UniformPointLightsBlock");
		if (uniformPointLightsBlockIndex == GL_INVALID_INDEX)
		{
			LogMessage (__func__) << "Program id" << shaderProgram.second.GetProgramId() << "has no UniformPointLightsBlock";
		}
		else {
			glUniformBlockBinding(shaderProgramId, uniformPointLightsBlockIndex, BindingPoint::BP_POINTLIGHTS);
		}

		uniformDirectionalLightsBlockIndex = glGetUniformBlockIndex (shaderProgramId, "UniformDirectionalLightsBlock");
		if (uniformDirectionalLightsBlockIndex == GL_INVALID_INDEX)
		{
			LogMessage (__func__) << "Program id" << shaderProgram.second.GetProgramId() << "has no UniformDirectionalLightsBlock";
		}
		else {
			glUniformBlockBinding(shaderProgramId, uniformDirectionalLightsBlockIndex, BindingPoint::BP_DIRECTIONALLIGHTS);
		}

		uniformSpotLightsBlockIndex = glGetUniformBlockIndex (shaderProgramId, "UniformSpotLightsBlock");
		if (uniformSpotLightsBlockIndex == GL_INVALID_INDEX)
		{
			LogMessage (__func__) << "Program id" << shaderProgram.second.GetProgramId() << "has no UniformSpotLightsBlock";
		}
		else {
			glUniformBlockBinding(shaderProgramId, uniformSpotLightsBlockIndex, BindingPoint::BP_SPOTLIGHTS);
		}

	}

	// scan objects vector for lights and populate light buffers
	for (auto gameObject : gameLoop->gameScene->GetGameObjects ())
	{
		if (gameObject.second->IsLight ())
		{
            gameObject.second->OnUpdate (*this);
        }
    }

}

void GLRenderer::UpdateLightBufferRecursive (unsigned int objectHandle)
{
	GameObject* gameObject = gameLoop->gameScene->GetGameObjects ()[objectHandle];

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
	activeCamera = gameLoop->gameScene->GetActiveCamera ();

	if (activeCamera == nullptr)
	{
		LogMessage (__func__, EXIT_FAILURE) << "No active camera found";
	}

	// load view matrix
	std::memcpy (uniformMatrixBuffer.viewMatrix, Mathf::Transpose (activeCamera->GetViewMatrix ())[0], 16 * sizeof (GLfloat));

	// load projection matrix
	std::memcpy (uniformMatrixBuffer.projectionMatrix, Mathf::Transpose (activeCamera->GetProjectionMatrix (gameLoop->gameRenderTarget->GetWidth (), gameLoop->gameRenderTarget->GetHeight ()))[0], 16 * sizeof (GLfloat));

	// load to GPU
	glBindBuffer (GL_UNIFORM_BUFFER, sceneBuffers.UBO[UBO_MATRICES]);
	glBufferSubData (GL_UNIFORM_BUFFER, 0, 2 * 16 * sizeof (GLfloat), &uniformMatrixBuffer);
	glBindBuffer (GL_UNIFORM_BUFFER, 0);

}


