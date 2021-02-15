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
#include "system/CallbackProvider.h"
#include "system/LogMessage.h"
#include "math/Mathf.h"

#include <unordered_map>
#include <cmath>

#include "graphics/default.vs.h"
#include "graphics/flatquad.vs.h"
#include "graphics/pbr.fs.h"
#include "graphics/phong.fs.h"
#include "graphics/blinnphong.fs.h"
#include "graphics/normals.fs.h"
#include "graphics/emissive.fs.h"
#include "graphics/flatquad.fs.h"
#include "graphics/post_hdr.fs.h"
#include "graphics/post_gamma.fs.h"

std::unordered_map<unsigned int, GLint> GLRenderer::textureChannelMap {
    {1, GL_RED},
    {3, GL_RGB},
    {4, GL_RGBA}
};

GLRenderer::GLRenderer (unsigned int width, unsigned int height) : Renderer (width, height)
{
#if (CILANTRO_GL_VERSION <= 140)
    framebuffer = new GLFramebuffer (width, heigh);
#else
    framebuffer = new GLMultisampleFramebuffer (width, height);
#endif
}

GLRenderer::~GLRenderer ()
{
    delete framebuffer;
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

    // initialize framebuffer
    framebuffer->Initialize ();

    // initialize shader library
    InitializeShaderLibrary ();

    // initialize object buffers
    InitializeObjectBuffers ();

    // initualize material texture units
    InitializeMaterialTextures ();

    // initialize uniform buffers of view and projection matrices
    InitializeMatrixUniformBuffers ();

    // initialize uniform buffers of lights
    InitializeLightUniformBuffers ();

    // set callback for new MeshObjects
    game->GetGameScene ().RegisterCallback ("OnUpdateMeshObject", [&](unsigned int objectHandle, unsigned int) { game->GetGameScene ().GetGameObjects ()[objectHandle]->OnUpdate (*this); });

    // set callback for new or modified lights
    game->GetGameScene ().RegisterCallback ("OnUpdateLight", [&](unsigned int objectHandle, unsigned int) { game->GetGameScene ().GetGameObjects ()[objectHandle]->OnUpdate (*this); });

    // set callback for new or modified materials
    game->GetGameScene ().RegisterCallback ("OnUpdateMaterial", [&](unsigned int materialHandle, unsigned int textureUnit) { this->Update (game->GetGameScene ().GetMaterialManager ().GetByHandle<Material> (materialHandle), textureUnit); });

    // set callback for modified scene graph (currently this only requires to reload light buffers)
    game->GetGameScene ().RegisterCallback ("OnUpdateSceneGraph", [&](unsigned int objectHandle, unsigned int) { UpdateLightBufferRecursive (objectHandle); });

    // set callback for modified transforms (currently this only requires to reload light buffers)
    game->GetGameScene ().RegisterCallback ("OnUpdateTransform", [&](unsigned int objectHandle, unsigned int) { UpdateLightBufferRecursive (objectHandle); });

    // check for any outstanding errors
    CheckGLError (MSG_LOCATION);

    LogMessage (MSG_LOCATION) << "GLRenderer started";
}

void GLRenderer::Deinitialize ()
{
    objectBuffers.clear ();
    pointLights.clear ();
    directionalLights.clear ();
    spotLights.clear ();
    shaders.clear ();
    shaderPrograms.clear ();

    framebuffer->Deinitialize ();
}

void GLRenderer::RenderFrame ()
{
    // bind framebuffer
    framebuffer->BindFramebuffer ();

    // clear frame and depth buffers
    glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // enable depth test
    glEnable (GL_DEPTH_TEST);

    // load uniform buffers
    LoadMatrixUniformBuffers ();

    // set viewport
    glViewport (0, 0, this->width, this->height);

    // draw all objects in scene
    for (auto&& gameObject : game->GetGameScene ().GetGameObjects ())
    {
        gameObject.second->OnDraw (*this);
    }

#if (CILANTRO_GL_VERSION > 140)
    // blit framebuffer
    framebuffer->BlitFramebuffer ();
#endif

    // base class functions
    Renderer::RenderFrame ();

    // check for errors
    CheckGLError (MSG_LOCATION);
}

void GLRenderer::SetResolution (unsigned int width, unsigned int height)
{
    Renderer::SetResolution (width, height);

    framebuffer->SetFramebufferResolution (width, height);

    for (auto&& postprocess : postprocesses)
    {
        dynamic_cast<GLFramebuffer*> (postprocess->GetFramebufferPtr ())->SetFramebufferResolution (width, height);
    }
}

GLuint GLRenderer::GetRendererFramebuffer () const
{
    if (postprocessStage == 0) 
    {
        return framebuffer->GetFramebuffer ();
    }
    else 
    {
        return dynamic_cast<GLFramebuffer*> (postprocesses[postprocessStage - 1]->GetFramebufferPtr ())->GetFramebuffer ();
    }
}

GLuint GLRenderer::GetRendererFramebufferTexture () const
{
    if (postprocessStage == 0) 
    {
        return framebuffer->GetFramebufferTexture ();
    }
    else 
    {
        return dynamic_cast<GLFramebuffer*> (postprocesses[postprocessStage - 1]->GetFramebufferPtr ())->GetFramebufferTexture ();
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
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Shader " << shaderName << " not found when adding to program " << shaderProgramName;
    }
    else
    {
        shaderPrograms[shaderProgramName].LinkShader (searchShader->second);
        if (searchProgram == shaderPrograms.end ())
        {
            LogMessage (MSG_LOCATION) << "Registered shader" << shaderProgramName << "with id" << shaderPrograms[shaderProgramName].GetProgramId ();
        }
    }
}

ShaderProgram& GLRenderer::GetShaderProgram (std::string shaderProgramName)
{
    auto searchProgram = shaderPrograms.find (shaderProgramName);

    if (searchProgram == shaderPrograms.end ())
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unable to find shader model" << shaderProgramName;
    }

    return searchProgram->second;
}

void GLRenderer::Draw (MeshObject& meshObject)
{
    GLuint eyePositionId;
    GLuint modelMatrixId;
    GLuint normalMatrixId;
    GLuint shaderProgramId;
    GLuint uniformId;

    // pick shader
    GLShaderProgram& shaderProgram = dynamic_cast<GLShaderProgram&>(GetShaderProgram (meshObject.GetMaterial ().GetShaderProgramName ()));
    shaderProgramId = shaderProgram.GetProgramId ();
    shaderProgram.Use ();

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
        glUniform3fv (eyePositionId, 1, &game->GetGameScene ().GetActiveCamera ()->GetPosition ()[0]);
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
    glDrawElements (GL_TRIANGLES, meshObject.GetIndexCount (), GL_UNSIGNED_INT, 0);
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

        LogMessage (MSG_LOCATION) << "New MeshObject" << objectHandle;

        // generate and bind Vertex Array Object (VAO)
        glGenVertexArrays (1, &objectBuffers[objectHandle].VAO);
        glBindVertexArray (objectBuffers[objectHandle].VAO);

        // generate vertex buffer and copy vertices to GPU
        glGenBuffers (1, &objectBuffers[objectHandle].VBO[VBOType::VBO_VERTICES]);
        glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_VERTICES]);
        glBufferData (GL_ARRAY_BUFFER, meshObject.GetVertexCount () * sizeof (float) * 3, meshObject.GetVerticesData (), GL_STATIC_DRAW);
        // location = 0 (vertex position)
        glVertexAttribPointer (VBOType::VBO_VERTICES, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate normals buffer and copy normals to GPU
        glGenBuffers (1, &objectBuffers[objectHandle].VBO[VBOType::VBO_NORMALS]);
        glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_NORMALS]);
        glBufferData (GL_ARRAY_BUFFER, meshObject.GetVertexCount () * sizeof (float) * 3, meshObject.GetNormalsData (), GL_STATIC_DRAW);
        // location = 1 (vertex normal)
        glVertexAttribPointer (VBOType::VBO_NORMALS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate uv buffer and copy uvs to GPU
        glGenBuffers (1, &objectBuffers[objectHandle].VBO[VBOType::VBO_UVS]);
        glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_UVS]);
        glBufferData (GL_ARRAY_BUFFER, meshObject.GetVertexCount () * sizeof (float) * 2, meshObject.GetUVData (), GL_STATIC_DRAW);
        // location = 2 (vertex uv)
        glVertexAttribPointer (VBOType::VBO_UVS, 2, GL_FLOAT, GL_FALSE, 2 * sizeof (float), (GLvoid*)0);

        // generate index buffer and copy face indices to GPU
        glGenBuffers (1, &objectBuffers[objectHandle].EBO);
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, objectBuffers[objectHandle].EBO);
        glBufferData (GL_ELEMENT_ARRAY_BUFFER, meshObject.GetIndexCount () * sizeof (unsigned int), meshObject.GetFacesData (), GL_STATIC_DRAW);

        // enable VBO arrays
        glEnableVertexAttribArray (VBOType::VBO_VERTICES);
        glEnableVertexAttribArray (VBOType::VBO_NORMALS);
        glEnableVertexAttribArray (VBOType::VBO_UVS);

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

        // load uv buffer
        glBindBuffer (GL_ARRAY_BUFFER, objectBuffers[objectHandle].VBO[VBOType::VBO_UVS]);
        glBufferData (GL_ARRAY_BUFFER, meshObject.GetVertexCount () * sizeof (float) * 2, meshObject.GetUVData (), GL_STATIC_DRAW);

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
        LogMessage (MSG_LOCATION) << "New PointLight" << objectHandle;
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
        LogMessage (MSG_LOCATION) << "New DirectionalLight" << objectHandle;
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
        LogMessage (MSG_LOCATION) << "New SpotLight" << objectHandle;
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
    GLShaderProgram& shader = dynamic_cast<GLShaderProgram&> (GetShaderProgram (material.GetShaderProgramName ()));
    GLuint shaderId = shader.GetProgramId ();
    GLuint texture;
    GLuint format;

    texture_map_t& textures = material.GetTexturesMap ();

    // check if material already in collection
    auto find = materialTextureUnits.find (materialHandle);

    if (find == materialTextureUnits.end ())
    {
        LogMessage (MSG_LOCATION) << "New Material texture units map" << materialHandle;

        materialTextureUnits.insert ({ materialHandle, MaterialTextureUnits() });
        
        for (auto&& t : textures)
        {
            Texture* tPtr = t.second.second;
            std::string tName = t.second.first;
            GLuint unit = t.first;
            format = textureChannelMap[tPtr->GetChannels ()];

            glGenTextures(1, &texture);
            LogMessage (MSG_LOCATION) << "Generate and bind texture" << texture << "name" << tName << "unit" << t.first << "[" << tPtr->GetWidth () << tPtr->GetHeight () << tPtr->GetChannels () << "]";
            glBindTexture(GL_TEXTURE_2D, texture);
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
        LogMessage (MSG_LOCATION) << "Update Material texture units map" << materialHandle;
        
        auto& t = textures[textureUnit];
        Texture* tPtr = t.second;
        std::string tName = t.first;
        GLuint unit = textureUnit;
        format = textureChannelMap[tPtr->GetChannels ()];

        glBindTexture(GL_TEXTURE_2D, materialTextureUnits[materialHandle].textureUnits[unit]);
        LogMessage (MSG_LOCATION) << "Bind texture" <<  materialTextureUnits[materialHandle].textureUnits[unit] << "name" << tName << "unit" << unit << "[" << tPtr->GetWidth () << tPtr->GetHeight () << tPtr->GetChannels () << "]";
        glTexImage2D(GL_TEXTURE_2D, 0, format, tPtr->GetWidth (), tPtr->GetHeight (), 0, format, GL_UNSIGNED_BYTE, tPtr->Data ());
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture (GL_TEXTURE_2D, 0);
        

    }

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
    AddShader ("pbr_fragment_shader", gPBRFragmentShader, ShaderType::FRAGMENT_SHADER);    
    AddShader ("phong_fragment_shader", gPhongFragmentShader, ShaderType::FRAGMENT_SHADER);
    AddShader ("blinnphong_fragment_shader", gBlinnPhongFragmentShader, ShaderType::FRAGMENT_SHADER);
    AddShader ("normals_fragment_shader", gNormalsFragmentShader, ShaderType::FRAGMENT_SHADER);
    AddShader ("emissive_fragment_shader", gEmissiveFragmentShader, ShaderType::FRAGMENT_SHADER);
    AddShader ("flatquad_fragment_shader", gFlatQuadFragmentShader, ShaderType::FRAGMENT_SHADER);
    AddShader ("post_hdr_fragment_shader", gPostHDRFragmentShader, ShaderType::FRAGMENT_SHADER);
    AddShader ("post_gamma_fragment_shader", gPostGammaFragmentShader, ShaderType::FRAGMENT_SHADER);

    // PBR model
    AddShaderToProgram ("pbr_shader", "default_vertex_shader");
    AddShaderToProgram ("pbr_shader", "pbr_fragment_shader");
#if (CILANTRO_GL_VERSION < 330)
    glBindAttribLocation(GetShaderProgram("pbr_shader").GetProgramId(), 0, "vPosition");
    glBindAttribLocation(GetShaderProgram("pbr_shader").GetProgramId(), 1, "vNormal");
#endif
#if (CILANTRO_GL_VERSION < 420)
    GLuint p = dynamic_cast<GLShaderProgram&>(GetShaderProgram("pbr_shader")).GetProgramId ();
    GetShaderProgram("pbr_shader").Use ();
    glUniform1i (glGetUniformLocation(p, "tAlbedo"), 0);
    glUniform1i (glGetUniformLocation(p, "tNormal"), 1);
    glUniform1i (glGetUniformLocation(p, "tMetallic"), 2);
    glUniform1i (glGetUniformLocation(p, "tRoughness"), 3);
    glUniform1i (glGetUniformLocation(p, "tAO"), 4);
#endif

    // Phong model
    AddShaderToProgram ("phong_shader", "default_vertex_shader");
    AddShaderToProgram ("phong_shader", "phong_fragment_shader");
#if (CILANTRO_GL_VERSION < 330)
    glBindAttribLocation(GetShaderProgram("phong_shader").GetProgramId(), 0, "vPosition");
    glBindAttribLocation(GetShaderProgram("phong_shader").GetProgramId(), 1, "vNormal");
#endif
#if (CILANTRO_GL_VERSION < 420)
    p = dynamic_cast<GLShaderProgram&>(GetShaderProgram("phong_shader")).GetProgramId ();
    GetShaderProgram("phong_shader").Use ();
    glUniform1i (glGetUniformLocation(p, "tDiffuse"), 0);
    glUniform1i (glGetUniformLocation(p, "tNormal"), 1);
    glUniform1i (glGetUniformLocation(p, "tSpecular"), 2);
    glUniform1i (glGetUniformLocation(p, "tEmissive"), 3);
#endif

    // Blinn-Phong model
    AddShaderToProgram ("blinnphong_shader", "default_vertex_shader");
    AddShaderToProgram ("blinnphong_shader", "blinnphong_fragment_shader");
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation(GetShaderProgram("blinnphong_shader").GetProgramId(), 0, "vPosition");
    glBindAttribLocation(GetShaderProgram("blinnphong_shader").GetProgramId(), 1, "vNormal");
#endif
#if (CILANTRO_GL_VERSION < 420)
    p = dynamic_cast<GLShaderProgram&>(GetShaderProgram("blinnphong_shader")).GetProgramId ();
    GetShaderProgram("blinnphong_shader").Use ();
    glUniform1i (glGetUniformLocation(p, "tDiffuse"), 0);
    glUniform1i (glGetUniformLocation(p, "tNormal"), 1);
    glUniform1i (glGetUniformLocation(p, "tSpecular"), 2);
    glUniform1i (glGetUniformLocation(p, "tEmissive"), 3);
#endif

    // Normals visualization model
    AddShaderToProgram ("normals_shader", "default_vertex_shader");
    AddShaderToProgram ("normals_shader", "normals_fragment_shader");
#if (CILANTRO_GL_VERSION < 330)
    glBindAttribLocation(GetShaderProgram("normals_shader").GetProgramId(), 0, "vPosition");
    glBindAttribLocation(GetShaderProgram("normals_shader").GetProgramId(), 1, "vNormal");
#endif

    // Only emissive color rendering (no lights calculation)
    AddShaderToProgram ("emissive_shader", "default_vertex_shader");
    AddShaderToProgram ("emissive_shader", "emissive_fragment_shader");
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation(GetShaderProgram("normals_shader").GetProgramId(), 0, "vPosition");
    glBindAttribLocation(GetShaderProgram("normals_shader").GetProgramId(), 1, "vNormal");
#endif
#if (CILANTRO_GL_VERSION < 420)
    p = dynamic_cast<GLShaderProgram&>(GetShaderProgram("emissive_shader")).GetProgramId ();
    GetShaderProgram("emissive_shader").Use ();
    glUniform1i (glGetUniformLocation(p, "tEmissive"), 3);
#endif

    // Screen quad rendering
    AddShaderToProgram ("flatquad_shader", "flatquad_vertex_shader");
    AddShaderToProgram ("flatquad_shader", "flatquad_fragment_shader");
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation(GetShaderProgram("flatquad_shader").GetProgramId(), 0, "vPosition");
    glBindAttribLocation(GetShaderProgram("flatquad_shader").GetProgramId(), 1, "vTextureCoordinates");
#endif
#if (CILANTRO_GL_VERSION < 420)
    p = dynamic_cast<GLShaderProgram&>(GetShaderProgram("flatquad_shader")).GetProgramId ();
    GetShaderProgram("flatquad_shader").Use ();
    glUniform1i (glGetUniformLocation(p, "fScreenTexture"), 0);
#endif

    // Post-processing HDR
    AddShaderToProgram ("post_hdr_shader", "flatquad_vertex_shader");
    AddShaderToProgram ("post_hdr_shader", "post_hdr_fragment_shader");
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation(GetShaderProgram("post_hdr_shader").GetProgramId(), 0, "vPosition");
    glBindAttribLocation(GetShaderProgram("post_hdr_shader").GetProgramId(), 1, "vTextureCoordinates");
#endif
#if (CILANTRO_GL_VERSION < 420)
    p = dynamic_cast<GLShaderProgram&>(GetShaderProgram("post_hdr_shader")).GetProgramId ();
    GetShaderProgram("post_hdr_shader").Use ();
    glUniform1i (glGetUniformLocation(p, "fScreenTexture"), 0);
#endif

    // Post-processing gamma
    AddShaderToProgram ("post_gamma_shader", "flatquad_vertex_shader");
    AddShaderToProgram ("post_gamma_shader", "post_gamma_fragment_shader");
#if (CILANTRO_GL_VERSION < 330)	
    glBindAttribLocation(GetShaderProgram("post_gamma_shader").GetProgramId(), 0, "vPosition");
    glBindAttribLocation(GetShaderProgram("post_gamma_shader").GetProgramId(), 1, "vTextureCoordinates");
#endif
#if (CILANTRO_GL_VERSION < 420)
    p = dynamic_cast<GLShaderProgram&>(GetShaderProgram("post_gamma_shader")).GetProgramId ();
    GetShaderProgram("post_gamma_shader").Use ();
    glUniform1i (glGetUniformLocation(p, "fScreenTexture"), 0);
#endif

}

void GLRenderer::InitializeObjectBuffers ()
{
    // load object buffers for all existing objects
    for (auto&& gameObject : game->GetGameScene ().GetGameObjects ())
    {
        // load buffers for MeshObject only
        if (dynamic_cast<MeshObject*>(gameObject.second) != nullptr)
        {
            gameObject.second->OnUpdate (*this);
        }
    }
}

void GLRenderer::InitializeMaterialTextures ()
{
    for (auto&& material : game->GetGameScene ().GetMaterialManager ())
    {
        this->Update (*material);
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
            LogMessage (MSG_LOCATION) << "Program id" << shaderProgram.second.GetProgramId() << "has no UniformMatricesBlock";
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
            LogMessage (MSG_LOCATION) << "Program id" << shaderProgram.second.GetProgramId() << "has no UniformPointLightsBlock";
        }
        else {
            glUniformBlockBinding(shaderProgramId, uniformPointLightsBlockIndex, BindingPoint::BP_POINTLIGHTS);
        }

        uniformDirectionalLightsBlockIndex = glGetUniformBlockIndex (shaderProgramId, "UniformDirectionalLightsBlock");
        if (uniformDirectionalLightsBlockIndex == GL_INVALID_INDEX)
        {
            LogMessage (MSG_LOCATION) << "Program id" << shaderProgram.second.GetProgramId() << "has no UniformDirectionalLightsBlock";
        }
        else {
            glUniformBlockBinding(shaderProgramId, uniformDirectionalLightsBlockIndex, BindingPoint::BP_DIRECTIONALLIGHTS);
        }

        uniformSpotLightsBlockIndex = glGetUniformBlockIndex (shaderProgramId, "UniformSpotLightsBlock");
        if (uniformSpotLightsBlockIndex == GL_INVALID_INDEX)
        {
            LogMessage (MSG_LOCATION) << "Program id" << shaderProgram.second.GetProgramId() << "has no UniformSpotLightsBlock";
        }
        else {
            glUniformBlockBinding(shaderProgramId, uniformSpotLightsBlockIndex, BindingPoint::BP_SPOTLIGHTS);
        }

    }

    // scan objects vector for lights and populate light buffers
    for (auto&& gameObject : game->GetGameScene ().GetGameObjects ())
    {
        if (gameObject.second->IsLight ())
        {
            gameObject.second->OnUpdate (*this);
        }
    }

}

void GLRenderer::UpdateLightBufferRecursive (unsigned int objectHandle)
{
    GameObject* gameObject = game->GetGameScene ().GetGameObjects ()[objectHandle];

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
    activeCamera = game->GetGameScene ().GetActiveCamera ();

    if (activeCamera == nullptr)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "No active camera found";
    }

    // load view matrix
    std::memcpy (uniformMatrixBuffer.viewMatrix, Mathf::Transpose (activeCamera->GetViewMatrix ())[0], 16 * sizeof (GLfloat));

    // load projection matrix
    std::memcpy (uniformMatrixBuffer.projectionMatrix, Mathf::Transpose (activeCamera->GetProjectionMatrix (this->GetWidth (), this->GetHeight ()))[0], 16 * sizeof (GLfloat));

    // load to GPU
    glBindBuffer (GL_UNIFORM_BUFFER, sceneBuffers.UBO[UBO_MATRICES]);
    glBufferSubData (GL_UNIFORM_BUFFER, 0, 2 * 16 * sizeof (GLfloat), &uniformMatrixBuffer);
    glBindBuffer (GL_UNIFORM_BUFFER, 0);

}


