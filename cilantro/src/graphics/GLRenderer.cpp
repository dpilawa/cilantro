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
#include "system/EngineContext.h"
#include "system/CallbackProvider.h"
#include "system/LogMessage.h"
#include "math/Mathf.h"

#include <unordered_map>
#include <cmath>

std::unordered_map<unsigned int, GLint> GLRenderer::textureChannelMap {
    {1, GL_RED},
    {3, GL_RGB},
    {4, GL_RGBA}
};

GLRenderer::GLRenderer (unsigned int width, unsigned int height) : Renderer (width, height)
{

}

GLRenderer::~GLRenderer ()
{
}

void GLRenderer::Initialize ()
{
    // set variable defaults
    postprocessStage = 0;

    // display GL version information
    LogMessage (MSG_LOCATION) << "Version:" << (char*) glGetString (GL_VERSION);
    LogMessage (MSG_LOCATION) << "Shader language version:" << (char*) glGetString (GL_SHADING_LANGUAGE_VERSION);
    LogMessage (MSG_LOCATION) << "Renderer:" << (char*) glGetString (GL_RENDERER);

    // enable depth test
    glEnable (GL_DEPTH_TEST);

    // enable face culling
    glFrontFace (GL_CCW);
    glEnable (GL_CULL_FACE);

    // enable multisampling
    glEnable (GL_MULTISAMPLE);

    // initialize object buffers
    InitializeObjectBuffers ();

    // initualize material texture units
    InitializeMaterialTextures ();

    // initialize uniform buffers of view and projection matrices
    InitializeMatrixUniformBuffers ();

    // initialize uniform buffers of lights
    InitializeLightUniformBuffers ();

    // set callback for new MeshObjects
    EngineContext::GetGameScene ().RegisterCallback ("OnUpdateMeshObject", [&](unsigned int objectHandle, unsigned int) { EngineContext::GetGameScene ().GetGameObjectManager ().GetByHandle<GameObject> (objectHandle).OnUpdate (*this); });

    // set callback for new or modified lights
    EngineContext::GetGameScene ().RegisterCallback ("OnUpdateLight", [&](unsigned int objectHandle, unsigned int) { EngineContext::GetGameScene ().GetGameObjectManager ().GetByHandle<GameObject> (objectHandle).OnUpdate (*this); });

    // set callback for new or modified materials
    EngineContext::GetGameScene ().RegisterCallback ("OnUpdateMaterial", [&](unsigned int materialHandle, unsigned int textureUnit) { this->Update (EngineContext::GetGameScene ().GetMaterialManager ().GetByHandle<Material> (materialHandle), textureUnit); });

    // set callback for modified scene graph (currently this only requires to reload light buffers)
    EngineContext::GetGameScene ().RegisterCallback ("OnUpdateSceneGraph", [&](unsigned int objectHandle, unsigned int) { UpdateLightBufferRecursive (objectHandle); });

    // set callback for modified transforms (currently this only requires to reload light buffers)
    EngineContext::GetGameScene ().RegisterCallback ("OnUpdateTransform", [&](unsigned int objectHandle, unsigned int) { UpdateLightBufferRecursive (objectHandle); });

    // initialize shader library
    InitializeShaderLibrary ();

    // check for any outstanding errors
    CheckGLError (MSG_LOCATION);

    LogMessage (MSG_LOCATION) << "GLRenderer started";

    Renderer::Initialize ();
}

void GLRenderer::Deinitialize ()
{
    objectBuffers.clear ();
    pointLights.clear ();
    directionalLights.clear ();
    spotLights.clear ();

    Renderer::Deinitialize ();
}

Framebuffer* GLRenderer::GetFramebuffer () const
{
    return framebuffer;
}

void GLRenderer::Draw (MeshObject& meshObject)
{
    GLuint eyePositionId;
    GLuint modelMatrixId;
    GLuint normalMatrixId;
    GLuint shaderProgramId;
    GLuint uniformId;

    // get shader program for rendered meshobject
    GLShaderProgram& shaderProgram = dynamic_cast<GLShaderProgram&>(GetMeshObjectShaderProgram (meshObject));
    shaderProgram.Use ();
    shaderProgramId = shaderProgram.GetProgramId ();

    // bind textures for active material
    if (materialTextureUnits.find(meshObject.GetMaterial ().GetHandle ()) != materialTextureUnits.end ())
    {
        MaterialTextureUnits u = materialTextureUnits[meshObject.GetMaterial ().GetHandle ()];

        for (GLuint i = 0; i < u.unitsCount; i++)
        {
            glActiveTexture (GL_TEXTURE0 + i);
            glBindTexture (GL_TEXTURE_2D, u.textureUnits[i]);
        }
    }
    else
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Missing texture for object" << meshObject.GetHandle ();
    }

    // set material uniforms for active material
    for (auto&& property : meshObject.GetMaterial ().GetPropertiesMap ())
    {
        uniformId = glGetUniformLocation (shaderProgramId, property.first.c_str ());
        if (uniformId != GL_INVALID_INDEX)
        {
            if (property.second.size () == 1)
            {
                glUniform1f (uniformId, property.second[0]);
            }
            else if ((property.second.size () == 3))
            {
                glUniform3fv (uniformId, 1, property.second.data ());
            }
            else
            {
                LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Invalid vector size for material property" << property.first;
            }
        }        
        else 
        {
            LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Invalid material uniform" << property.first;
        }
    }

    // get camera position in world space and set uniform value
    eyePositionId = glGetUniformLocation (shaderProgramId, "eyePosition");
    if (eyePositionId != GL_INVALID_INDEX)
    {
        glUniform3fv (eyePositionId, 1, &EngineContext::GetGameScene ().GetActiveCamera ()->GetPosition ()[0]);
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
    glDrawElements (GL_TRIANGLES, meshObject.GetMesh ().GetIndexCount (), GL_UNSIGNED_INT, 0);
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

        // generate and bind Vertex Array Object (VAO)
        glGenVertexArrays (1, &objectBuffers[objectHandle].VAO);
        glBindVertexArray (objectBuffers[objectHandle].VAO);

        // generate vertex buffer and copy vertices to GPU
        glGenBuffers (1, &objectBuffers[objectHandle].VBO[VBOType::VBO_VERTICES]);
        glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_VERTICES]);
        glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 3, meshObject.GetMesh ().GetVerticesData (), GL_STATIC_DRAW);
        // location = 0 (vertex position)
        glVertexAttribPointer (VBOType::VBO_VERTICES, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate normals buffer and copy normals to GPU
        glGenBuffers (1, &objectBuffers[objectHandle].VBO[VBOType::VBO_NORMALS]);
        glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_NORMALS]);
        glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 3, meshObject.GetMesh ().GetNormalsData (), GL_STATIC_DRAW);
        // location = 1 (vertex normal)
        glVertexAttribPointer (VBOType::VBO_NORMALS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate uv buffer and copy uvs to GPU
        glGenBuffers (1, &objectBuffers[objectHandle].VBO[VBOType::VBO_UVS]);
        glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_UVS]);
        glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 2, meshObject.GetMesh ().GetUVData (), GL_STATIC_DRAW);
        // location = 2 (vertex uv)
        glVertexAttribPointer (VBOType::VBO_UVS, 2, GL_FLOAT, GL_FALSE, 2 * sizeof (float), (GLvoid*)0);

        // generate tangents buffer and copy tangents to GPU
        glGenBuffers (1, &objectBuffers[objectHandle].VBO[VBOType::VBO_TANGENTS]);
        glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_TANGENTS]);
        glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 3, meshObject.GetMesh ().GetTangentData (), GL_STATIC_DRAW);
        // location = 3 (vertex tangent)
        glVertexAttribPointer (VBOType::VBO_TANGENTS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate bitangents buffer and copy tangents to GPU
        glGenBuffers (1, &objectBuffers[objectHandle].VBO[VBOType::VBO_BITANGENTS]);
        glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_BITANGENTS]);
        glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 3, meshObject.GetMesh ().GetBitangentData (), GL_STATIC_DRAW);
        // location = 4 (vertex bitangent)
        glVertexAttribPointer (VBOType::VBO_BITANGENTS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate index buffer and copy face indices to GPU
        glGenBuffers (1, &objectBuffers[objectHandle].EBO);
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, objectBuffers[objectHandle].EBO);
        glBufferData (GL_ELEMENT_ARRAY_BUFFER, meshObject.GetMesh ().GetIndexCount () * sizeof (unsigned int), meshObject.GetMesh ().GetFacesData (), GL_STATIC_DRAW);

        // enable VBO arrays
        glEnableVertexAttribArray (VBOType::VBO_VERTICES);
        glEnableVertexAttribArray (VBOType::VBO_NORMALS);
        glEnableVertexAttribArray (VBOType::VBO_UVS);
        glEnableVertexAttribArray (VBOType::VBO_TANGENTS);
        glEnableVertexAttribArray (VBOType::VBO_BITANGENTS);

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
        glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 3, meshObject.GetMesh ().GetVerticesData (), GL_STATIC_DRAW);

        // load normals buffer
        glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_NORMALS]);
        glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 3, meshObject.GetMesh ().GetNormalsData (), GL_STATIC_DRAW);

        // load uv buffer
        glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_UVS]);
        glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 2, meshObject.GetMesh ().GetUVData (), GL_STATIC_DRAW);

        // load tangents buffer
        glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_TANGENTS]);
        glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 3, meshObject.GetMesh ().GetTangentData (), GL_STATIC_DRAW);

        // load bitangents buffer
        glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_BITANGENTS]);
        glBufferData (GL_ARRAY_BUFFER, meshObject.GetMesh ().GetVertexCount () * sizeof (float) * 3, meshObject.GetMesh ().GetBitangentData (), GL_STATIC_DRAW);

        // load index buffer
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, objectBuffers[objectHandle].EBO);
        glBufferData (GL_ELEMENT_ARRAY_BUFFER, meshObject.GetMesh ().GetIndexCount () * sizeof (unsigned int), meshObject.GetMesh ().GetFacesData (), GL_STATIC_DRAW);

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

void GLRenderer::Update (Material& material, unsigned int textureUnit)
{
    unsigned int materialHandle = material.GetHandle ();
    GLuint texture;
    GLuint format;

    texture_map_t& textures = material.GetTexturesMap ();

    // check if material already in collection
    auto find = materialTextureUnits.find (materialHandle);

    if (find == materialTextureUnits.end ())
    {
        materialTextureUnits.insert ({ materialHandle, MaterialTextureUnits () });
        
        for (auto&& t : textures)
        {
            Texture* tPtr = t.second.second;
            std::string tName = t.second.first;
            GLuint unit = t.first;
            format = textureChannelMap[tPtr->GetChannels ()];

            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, format, tPtr->GetWidth (), tPtr->GetHeight (), 0, format, GL_UNSIGNED_BYTE, tPtr->Data ());
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture (GL_TEXTURE_2D, 0);
            
            materialTextureUnits[materialHandle].textureUnits[unit] = texture;
        }

        materialTextureUnits[materialHandle].unitsCount = textures.size ();

    }
    else
    {
        auto& t = textures[textureUnit];
        Texture* tPtr = t.second;
        std::string tName = t.first;
        GLuint unit = textureUnit;
        format = textureChannelMap[tPtr->GetChannels ()];

        glBindTexture(GL_TEXTURE_2D, materialTextureUnits[materialHandle].textureUnits[unit]);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, tPtr->GetWidth (), tPtr->GetHeight (), 0, format, GL_UNSIGNED_BYTE, tPtr->Data ());
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture (GL_TEXTURE_2D, 0);
    }

}

void GLRenderer::CheckGLError (const std::string& functionName)
{
    GLuint errorCode;

    if ((errorCode = glGetError ()) != GL_NO_ERROR)
    {
        LogMessage (functionName, EXIT_FAILURE) << "glError:" << std::hex << std::showbase << errorCode;
    }
}

void GLRenderer::InitializeShaderLibrary ()
{
    GLuint p;

    // initialize shader library
    EngineContext::GetResourceManager ().Load<GLShader> ("default_vertex_shader", "shaders/default.vs", ShaderType::VERTEX_SHADER);
    EngineContext::GetResourceManager ().Load<GLShader> ("flatquad_vertex_shader", "shaders/flatquad.vs", ShaderType::VERTEX_SHADER);
    EngineContext::GetResourceManager ().Load<GLShader> ("pbr_forward_fragment_shader", "shaders/pbr_forward.fs", ShaderType::FRAGMENT_SHADER);    
    EngineContext::GetResourceManager ().Load<GLShader> ("pbr_deferred_geometrypass_fragment_shader", "shaders/pbr_deferred_geometrypass.fs", ShaderType::FRAGMENT_SHADER); 
    EngineContext::GetResourceManager ().Load<GLShader> ("pbr_deferred_lightingpass_fragment_shader", "shaders/pbr_deferred_lightingpass.fs", ShaderType::FRAGMENT_SHADER); 
    EngineContext::GetResourceManager ().Load<GLShader> ("phong_forward_fragment_shader", "shaders/phong_forward.fs", ShaderType::FRAGMENT_SHADER);
    EngineContext::GetResourceManager ().Load<GLShader> ("blinnphong_forward_fragment_shader", "shaders/blinnphong_forward.fs", ShaderType::FRAGMENT_SHADER);
    EngineContext::GetResourceManager ().Load<GLShader> ("blinnphong_deferred_geometrypass_fragment_shader", "shaders/blinnphong_deferred_geometrypass.fs", ShaderType::FRAGMENT_SHADER); 
    EngineContext::GetResourceManager ().Load<GLShader> ("blinnphong_deferred_lightingpass_fragment_shader", "shaders/blinnphong_deferred_lightingpass.fs", ShaderType::FRAGMENT_SHADER); 
    EngineContext::GetResourceManager ().Load<GLShader> ("flatquad_fragment_shader", "shaders/flatquad.fs", ShaderType::FRAGMENT_SHADER);
    EngineContext::GetResourceManager ().Load<GLShader> ("post_hdr_fragment_shader", "shaders/post_hdr.fs", ShaderType::FRAGMENT_SHADER);
    EngineContext::GetResourceManager ().Load<GLShader> ("post_gamma_fragment_shader", "shaders/post_gamma.fs", ShaderType::FRAGMENT_SHADER);

    // PBR model (forward)
    AddShaderProgram<GLShaderProgram> ("pbr_forward_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("pbr_forward_shader").AddShader ("default_vertex_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("pbr_forward_shader").AddShader ("pbr_forward_fragment_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("pbr_forward_shader").BindUniformBlock ("UniformMatricesBlock", BindingPoint::BP_MATRICES);
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("pbr_forward_shader").BindUniformBlock ("UniformPointLightsBlock", BindingPoint::BP_POINTLIGHTS);
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("pbr_forward_shader").BindUniformBlock ("UniformDirectionalLightsBlock", BindingPoint::BP_DIRECTIONALLIGHTS);
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("pbr_forward_shader").BindUniformBlock ("UniformSpotLightsBlock", BindingPoint::BP_SPOTLIGHTS);

    p = GetShaderProgramManager ().GetByName<GLShaderProgram> ("pbr_forward_shader").GetProgramId ();
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("pbr_forward_shader").Use ();
#if (CILANTRO_GL_VERSION < 330)
    glBindAttribLocation(p, 0, "vPosition");
    glBindAttribLocation(p, 1, "vNormal");
    glBindAttribLocation(p, 2, "vUV");
    glBindAttribLocation(p, 3, "vTangent");
    glBindAttribLocation(p, 4, "vBitangent");
#endif
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p, "tAlbedo"), 0);
    glUniform1i (glGetUniformLocation (p, "tNormal"), 1);
    glUniform1i (glGetUniformLocation (p, "tMetallic"), 2);
    glUniform1i (glGetUniformLocation (p, "tRoughness"), 3);
    glUniform1i (glGetUniformLocation (p, "tAO"), 4);
#endif

    // PBR model (deferred, geometry pass)
    AddShaderProgram<GLShaderProgram> ("pbr_deferred_geometrypass_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("pbr_deferred_geometrypass_shader").AddShader ("default_vertex_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("pbr_deferred_geometrypass_shader").AddShader ("pbr_deferred_geometrypass_fragment_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("pbr_deferred_geometrypass_shader").BindUniformBlock ("UniformMatricesBlock", BindingPoint::BP_MATRICES);

    p = GetShaderProgramManager ().GetByName<GLShaderProgram> ("pbr_deferred_geometrypass_shader").GetProgramId ();
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("pbr_deferred_geometrypass_shader").Use ();
#if (CILANTRO_GL_VERSION < 330)
    glBindAttribLocation(p, 0, "vPosition");
    glBindAttribLocation(p, 1, "vNormal");
    glBindAttribLocation(p, 2, "vUV");
    glBindAttribLocation(p, 3, "vTangent");
    glBindAttribLocation(p, 4, "vBitangent");
#endif
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p, "tAlbedo"), 0);
    glUniform1i (glGetUniformLocation (p, "tNormal"), 1);
    glUniform1i (glGetUniformLocation (p, "tMetallic"), 2);
    glUniform1i (glGetUniformLocation (p, "tRoughness"), 3);
    glUniform1i (glGetUniformLocation (p, "tAO"), 4);
#endif

    // PBR model (deferred, lighting pass)
    AddShaderProgram<GLShaderProgram> ("pbr_deferred_lightingpass_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("pbr_deferred_lightingpass_shader").AddShader ("flatquad_vertex_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("pbr_deferred_lightingpass_shader").AddShader ("pbr_deferred_lightingpass_fragment_shader");

    p = GetShaderProgramManager ().GetByName<GLShaderProgram> ("pbr_deferred_lightingpass_shader").GetProgramId ();
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("pbr_deferred_lightingpass_shader").Use ();
#if (CILANTRO_GL_VERSION < 330)
    glBindAttribLocation(p, 0, "vPosition");
    glBindAttribLocation(p, 1, "vTextureCoordinates");
#endif
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p, "tPosition"), 0);
    glUniform1i (glGetUniformLocation (p, "tNormal"), 1);
    glUniform1i (glGetUniformLocation (p, "tMetallic"), 2);
    glUniform1i (glGetUniformLocation (p, "tRoughness"), 3);
    glUniform1i (glGetUniformLocation (p, "tAlbedoAO"), 4);
#endif

    // Phong model (forward)
    AddShaderProgram<GLShaderProgram> ("phong_forward_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("phong_forward_shader").AddShader ("default_vertex_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("phong_forward_shader").AddShader ("phong_forward_fragment_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("phong_forward_shader").BindUniformBlock ("UniformMatricesBlock", BindingPoint::BP_MATRICES);
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("phong_forward_shader").BindUniformBlock ("UniformPointLightsBlock", BindingPoint::BP_POINTLIGHTS);
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("phong_forward_shader").BindUniformBlock ("UniformDirectionalLightsBlock", BindingPoint::BP_DIRECTIONALLIGHTS);
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("phong_forward_shader").BindUniformBlock ("UniformSpotLightsBlock", BindingPoint::BP_SPOTLIGHTS);

    p = GetShaderProgramManager ().GetByName<GLShaderProgram> ("phong_forward_shader").GetProgramId ();
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("phong_forward_shader").Use ();
#if (CILANTRO_GL_VERSION < 330)
    glBindAttribLocation(p, 0, "vPosition");
    glBindAttribLocation(p, 1, "vNormal");
    glBindAttribLocation(p, 2, "vUV");
    glBindAttribLocation(p, 3, "vTangent");
    glBindAttribLocation(p, 4, "vBitangent");
#endif
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p, "tDiffuse"), 0);
    glUniform1i (glGetUniformLocation (p, "tNormal"), 1);
    glUniform1i (glGetUniformLocation (p, "tSpecular"), 2);
    glUniform1i (glGetUniformLocation (p, "tEmissive"), 3);
#endif

    // Blinn-Phong model (forward)
    AddShaderProgram<GLShaderProgram> ("blinnphong_forward_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("blinnphong_forward_shader").AddShader ("default_vertex_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("blinnphong_forward_shader").AddShader ("blinnphong_forward_fragment_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("blinnphong_forward_shader").BindUniformBlock ("UniformMatricesBlock", BindingPoint::BP_MATRICES);
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("blinnphong_forward_shader").BindUniformBlock ("UniformPointLightsBlock", BindingPoint::BP_POINTLIGHTS);
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("blinnphong_forward_shader").BindUniformBlock ("UniformDirectionalLightsBlock", BindingPoint::BP_DIRECTIONALLIGHTS);
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("blinnphong_forward_shader").BindUniformBlock ("UniformSpotLightsBlock", BindingPoint::BP_SPOTLIGHTS);    
    
    p = GetShaderProgramManager ().GetByName<GLShaderProgram> ("blinnphong_forward_shader").GetProgramId ();
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("blinnphong_forward_shader").Use ();
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation(p, 0, "vPosition");
    glBindAttribLocation(p, 1, "vNormal");
    glBindAttribLocation(p, 2, "vUV");
    glBindAttribLocation(p, 3, "vTangent");
    glBindAttribLocation(p, 4, "vBitangent");
#endif
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p, "tDiffuse"), 0);
    glUniform1i (glGetUniformLocation (p, "tNormal"), 1);
    glUniform1i (glGetUniformLocation (p, "tSpecular"), 2);
    glUniform1i (glGetUniformLocation (p, "tEmissive"), 3);
#endif

    // Blinn-Phong model (deferred, geometry pass)
    AddShaderProgram<GLShaderProgram> ("blinnphong_deferred_geometrypass_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("blinnphong_deferred_geometrypass_shader").AddShader ("default_vertex_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("blinnphong_deferred_geometrypass_shader").AddShader ("blinnphong_deferred_geometrypass_fragment_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("blinnphong_deferred_geometrypass_shader").BindUniformBlock ("UniformMatricesBlock", BindingPoint::BP_MATRICES);

    p = GetShaderProgramManager ().GetByName<GLShaderProgram> ("blinnphong_deferred_geometrypass_shader").GetProgramId ();
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("blinnphong_deferred_geometrypass_shader").Use ();
#if (CILANTRO_GL_VERSION < 330)
    glBindAttribLocation(p, 0, "vPosition");
    glBindAttribLocation(p, 1, "vNormal");
    glBindAttribLocation(p, 2, "vUV");
    glBindAttribLocation(p, 3, "vTangent");
    glBindAttribLocation(p, 4, "vBitangent");
#endif
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p, "tDiffuse"), 0);
    glUniform1i (glGetUniformLocation (p, "tNormal"), 1);
    glUniform1i (glGetUniformLocation (p, "tSpecular"), 2);
    glUniform1i (glGetUniformLocation (p, "tEmissive"), 3);
#endif

    // Blinn-Phong model (deferred, lighting pass)
    AddShaderProgram<GLShaderProgram> ("blinnphong_deferred_lightingpass_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("blinnphong_deferred_lightingpass_shader").AddShader ("flatquad_vertex_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("blinnphong_deferred_lightingpass_shader").AddShader ("blinnphong_deferred_lightingpass_fragment_shader");

    p = GetShaderProgramManager ().GetByName<GLShaderProgram> ("blinnphong_deferred_lightingpass_shader").GetProgramId ();
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("blinnphong_deferred_lightingpass_shader").Use ();
#if (CILANTRO_GL_VERSION < 330)
    glBindAttribLocation(p, 0, "vPosition");
    glBindAttribLocation(p, 1, "vTextureCoordinates");
#endif
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p, "tPosition"), 0);
    glUniform1i (glGetUniformLocation (p, "tNormal"), 1);
    glUniform1i (glGetUniformLocation (p, "tEmissive"), 2);
    glUniform1i (glGetUniformLocation (p, "tUnused"), 3);
    glUniform1i (glGetUniformLocation (p, "tSpecular"), 4);
#endif

    // Screen quad rendering
    AddShaderProgram<GLShaderProgram> ("flatquad_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("flatquad_shader").AddShader ("flatquad_vertex_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("flatquad_shader").AddShader ("flatquad_fragment_shader");

    p = GetShaderProgramManager ().GetByName<GLShaderProgram> ("flatquad_shader").GetProgramId ();
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("flatquad_shader").Use ();    
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation(p, 0, "vPosition");
    glBindAttribLocation(p, 1, "vTextureCoordinates");
#endif
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p, "fScreenTexture"), 0);
#endif

    // Post-processing HDR
    AddShaderProgram<GLShaderProgram> ("post_hdr_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("post_hdr_shader").AddShader ("flatquad_vertex_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("post_hdr_shader").AddShader ("post_hdr_fragment_shader");

    p = GetShaderProgramManager ().GetByName<GLShaderProgram> ("post_hdr_shader").GetProgramId ();
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("post_hdr_shader").Use ();
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation(p, 0, "vPosition");
    glBindAttribLocation(p, 1, "vTextureCoordinates");
#endif
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p, "fScreenTexture"), 0);
#endif

    // Post-processing gamma
    AddShaderProgram<GLShaderProgram> ("post_gamma_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("post_gamma_shader").AddShader ("flatquad_vertex_shader");
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("post_gamma_shader").AddShader ("post_gamma_fragment_shader");
    
    p = GetShaderProgramManager ().GetByName<GLShaderProgram> ("post_gamma_shader").GetProgramId ();
    GetShaderProgramManager ().GetByName<GLShaderProgram> ("post_gamma_shader").Use ();    
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation(p, 0, "vPosition");
    glBindAttribLocation(p, 1, "vTextureCoordinates");
#endif
#if (CILANTRO_GL_VERSION < 420)
    glUniform1i (glGetUniformLocation (p, "fScreenTexture"), 0);
#endif

}

void GLRenderer::InitializeObjectBuffers ()
{
    // load object buffers for all existing objects
    for (auto gameObject : EngineContext::GetGameScene ().GetGameObjectManager ())
    {
        // load buffers for MeshObject only
        if (std::dynamic_pointer_cast<MeshObject> (gameObject) != nullptr)
        {
            gameObject->OnUpdate (*this);
        }      
    }
}

void GLRenderer::InitializeMaterialTextures ()
{
    for (auto&& material : EngineContext::GetGameScene ().GetMaterialManager ())
    {
        this->Update (*material);
    }
}

void GLRenderer::InitializeMatrixUniformBuffers ()
{
    // create and pre-load uniform buffer for view and projection matrices
    glGenBuffers (1, &sceneBuffers.UBO[UBO_MATRICES]);
    glBindBuffer (GL_UNIFORM_BUFFER, sceneBuffers.UBO[UBO_MATRICES]);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (UniformMatrixBuffer), &uniformMatrixBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, BindingPoint::BP_MATRICES, sceneBuffers.UBO[UBO_MATRICES]);
}

void GLRenderer::InitializeLightUniformBuffers ()
{
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

    // scan objects vector for lights and populate light buffers
    for (auto gameObject : EngineContext::GetGameScene ().GetGameObjectManager ())
    {
        if (std::dynamic_pointer_cast<Light> (gameObject) != nullptr)
        {
            gameObject->OnUpdate (*this);
        }
    }

}

void GLRenderer::UpdateLightBufferRecursive (unsigned int objectHandle)
{
    GameObject* light = &EngineContext::GetGameScene ().GetGameObjectManager ().GetByHandle<GameObject> (objectHandle);

    if (dynamic_cast<Light*>(light) != nullptr)
    {
        light->OnUpdate (*this);
    }

    for (auto& childObject : light->GetChildObjects ())
    {
        UpdateLightBufferRecursive (childObject->GetHandle ());
    }

}

void GLRenderer::LoadMatrixUniformBuffers ()
{
    Camera* activeCamera;

    // get active camera of rendered scene
    activeCamera = EngineContext::GetGameScene ().GetActiveCamera ();

    if (activeCamera == nullptr)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "No active camera found";
    }

    // load view matrix
    std::memcpy (uniformMatrixBuffer.viewMatrix, Mathf::Transpose (activeCamera->GetViewMatrix ())[0], 16 * sizeof (GLfloat));

    // load projection matrix
    std::memcpy (uniformMatrixBuffer.projectionMatrix, Mathf::Transpose (activeCamera->GetProjectionMatrix (this->GetFramebuffer ()->GetWidth (), this->GetFramebuffer ()->GetHeight ()))[0], 16 * sizeof (GLfloat));

    // load to GPU
    glBindBuffer (GL_UNIFORM_BUFFER, sceneBuffers.UBO[UBO_MATRICES]);
    glBufferSubData (GL_UNIFORM_BUFFER, 0, 2 * 16 * sizeof (GLfloat), &uniformMatrixBuffer);
    glBindBuffer (GL_UNIFORM_BUFFER, 0);

}


