#include "graphics/GLRenderer.h"
#include "graphics/ShaderProgramNames.h"
#include "graphics/GLUtils.h"
#include "graphics/GLShaderProgram.h"
#include "graphics/GLShaderLibrary.h"
#include "graphics/GLCameraBuffer.h"
#include "graphics/GLLightBuffers.h"
#include "graphics/GLGeometryStore.h"
#include "graphics/GLMaterialBindings.h"
#include "graphics/GLFramebuffer.h"
#include "graphics/GLMultisampleFramebuffer.h"

#include "system/Game.h"
#include "math/Mathf.h"
#include "math/Vector4f.h"
#include "math/Matrix3f.h"
#include "math/Matrix4f.h"
#include "scene/GameScene.h"
#include "scene/MeshObject.h"
#include "scene/Camera.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "scene/SpotLight.h"

namespace cilantro {

GLRenderer::GLRenderer (std::shared_ptr<GameScene> gameScene, unsigned int width, unsigned int height, bool shadowMappingEnabled, bool deferredRenderingEnabled) 
    : Renderer (gameScene, width, height, shadowMappingEnabled, deferredRenderingEnabled)
{
    m_geometryStore = std::make_unique<GLGeometryStore> ();
    m_cameraBuffer = std::make_unique<GLCameraBuffer> ();
    m_lightBuffers = std::make_unique<GLLightBuffers> ();
    m_materialBindings = std::make_unique<GLMaterialBindings> ();
}

GLRenderer::~GLRenderer ()
{
}

void GLRenderer::Initialize ()
{    
    Renderer::Initialize ();

    GLUtils::PrintGLInfo ();
    GLUtils::PrintGLExtensions ();

    m_shaderLibrary = std::make_unique<GLShaderLibrary> (GetGameScene ()->GetGame ()->GetResourceManager (), m_shaderProgramManager);
    m_shaderLibrary->Initialize ();
    m_geometryStore->Initialize ();
    InitializeObjectBuffers ();
    m_cameraBuffer->Initialize ();
    InitializeLightUniformBuffers ();

    SubscribeToSceneMessages ();
}

void GLRenderer::Deinitialize ()
{
    Renderer::Deinitialize ();

    m_geometryStore->Deinitialize ();
    m_cameraBuffer->Deinitialize ();
    m_materialBindings->Deinitialize ();
    DeinitializeLightUniformBuffers ();
}

std::shared_ptr<IRenderer> GLRenderer::SetViewport (unsigned int x, unsigned int y, unsigned int sx, unsigned int sy)
{
    glViewport (x, y, sx, sy);

    return std::dynamic_pointer_cast<IRenderer> (shared_from_this ());
}

void GLRenderer::Draw (std::shared_ptr<MeshObject> meshObject)
{
    GLuint shaderProgramId;

    auto objM = meshObject->GetMaterial ();

    // get shader program for rendered meshobject (geometry pass)
    auto geometryShaderProgram = m_shaderProgramManager->GetByName<GLShaderProgram> (
        m_isDeferredRendering
        ? objM->GetDeferredGeometryPassShaderProgram ()
        : objM->GetForwardShaderProgram ()
    );
    geometryShaderProgram->Use ();
    shaderProgramId = geometryShaderProgram->GetProgramId ();

    // bind textures for active material and bind a shadow map
    if (m_materialBindings->Bind (meshObject->GetMaterial ()))
    {
        // bind shadow maps (if exist)
        if (m_isShadowMapping && (GetCurrentRenderStage ()->GetLinkedDepthTextureArrayFramebuffer ()) != nullptr)
        {
            if (GetCurrentRenderStage ()->GetLinkedDepthTextureArrayFramebuffer ()->IsDepthTextureArrayEnabled ())
            {
                GetCurrentRenderStage ()->GetLinkedDepthTextureArrayFramebuffer ()->BindFramebufferDepthTextureArrayAsColor (CILANTRO_SHADOW_MAP_BINDING);
            }
        }

    }
    else
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Missing texture for object" << meshObject->GetHandle ();
    }

    // set material uniforms for active material
    for (auto&& property : meshObject->GetMaterial ()->GetPropertiesMap ())
    {
        if (geometryShaderProgram->HasUniform (property.first.c_str ()))
        {
            if (property.second.size () == 1)
            {
                geometryShaderProgram->SetUniformFloat (property.first.c_str (), property.second[0]);
            }
            else if ((property.second.size () == 3))
            {
                geometryShaderProgram->SetUniformFloatv (property.first.c_str (), property.second.data (), 3);
            }
            else
            {
                LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Invalid vector size for material property" << property.first << "in shader" << geometryShaderProgram->GetName () << "for" << meshObject->GetName ();
            }
        }        
        else 
        {
            LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Invalid material uniform" << property.first << "in shader" << geometryShaderProgram->GetName () << "for" << meshObject->GetName ();
        }
    }

    // get world matrix for drawn objects and set uniform value
    geometryShaderProgram->SetUniformMatrix4f ("mModel", meshObject->GetWorldTransformMatrix ());

    // calculate normal matrix for drawn objects and set uniform value
    geometryShaderProgram->SetUniformMatrix3f ("mNormal", Mathf::Invert (Mathf::Transpose (Matrix3f (meshObject->GetWorldTransformMatrix ()))));

    // set shadow map uniform (if shadow mapping is enabled)
    // this is only required for forward rendering, because deferred rendering uses a different shader program for lighting pass (DeferredLightingRenderStage)
    if (!m_isDeferredRendering)
    {
        geometryShaderProgram->SetUniformInt ("shadowMapEnabled", m_isShadowMapping ? 1 : 0);
    }
    
    // get camera position in world space and set uniform value
    if (!m_isDeferredRendering)
    {
        geometryShaderProgram->SetUniformVector3f ("eyePosition", GetGameScene ()->GetActiveCamera ()->GetPosition ());
    }

    // get shader program for rendered meshobject (lighting pass)
    if (m_isDeferredRendering)
    {
        auto lightingShaderProgram = m_shaderProgramManager->GetByName<GLShaderProgram>(objM->GetDeferredLightingPassShaderProgram ());
        lightingShaderProgram->Use ();
        shaderProgramId = lightingShaderProgram->GetProgramId ();

        // get camera position in world space and set uniform value (this needs to be done again for deferred lighting shader program)
        lightingShaderProgram->SetUniformVector3f ("eyePosition", GetGameScene ()->GetActiveCamera ()->GetPosition ());

    }

    // draw mesh
    geometryShaderProgram->Use ();
    m_geometryStore->Draw (meshObject);
}

void GLRenderer::DrawSurface ()
{
    m_geometryStore->DrawSurface ();
}

void GLRenderer::DrawSceneGeometryBuffers (std::shared_ptr<IShaderProgram> shader)
{
    m_geometryStore->DrawAll (shader, GetGameScene ()->GetGameObjectManager ());
}

void GLRenderer::DrawAABBGeometryBuffers (std::shared_ptr<IShaderProgram> shader)
{
    m_geometryStore->DrawAABBs (shader);
}

void GLRenderer::Update (std::shared_ptr<MeshObject> meshObject)
{
    m_geometryStore->Update (meshObject);
}

void GLRenderer::UpdateAABBBuffers (std::shared_ptr<MeshObject> meshObject)
{
    m_geometryStore->UpdateAABB (meshObject);
}

AABB GLRenderer::CalculateAABB (std::shared_ptr<MeshObject> meshObject)
{
    if (GLUtils::GetGLSLVersion ().versionNumber >= 430)
    {
        // calculate in GPU
        return m_geometryStore->CalculateAABB (meshObject, m_shaderProgramManager->GetByName<GLShaderProgram> (ShaderProgramNames::AABBCompute));
    }
    else
    {
        // fall back to CPU calculation
        return Renderer::CalculateAABB (meshObject);
    }
}

void GLRenderer::Update (std::shared_ptr<Material> material, unsigned int textureUnit)
{
    m_materialBindings->Update (material, textureUnit);
}

void GLRenderer::Update (std::shared_ptr<PointLight> pointLight)
{
    if (m_lightBuffers->Update (pointLight))
    {
        m_shaderLibrary->OnPointLightAdded (GetDirectionalLightCount (), GetSpotLightCount (), GetPointLightCount ());
    }
}

void GLRenderer::Update (std::shared_ptr<DirectionalLight> directionalLight)
{
    if (m_lightBuffers->Update (directionalLight))
    {
        m_shaderLibrary->OnDirectionalLightAdded (GetDirectionalLightCount (), GetSpotLightCount (), GetPointLightCount ());
    }
}

void GLRenderer::Update (std::shared_ptr<SpotLight> spotLight)
{
    if (m_lightBuffers->Update (spotLight))
    {
        m_shaderLibrary->OnSpotLightAdded (GetDirectionalLightCount (), GetSpotLightCount (), GetPointLightCount ());
    }
}

void GLRenderer::UpdateCameraBuffers (std::shared_ptr<Camera> camera)
{
    m_cameraBuffer->Update (camera, m_width, m_height);
}

void GLRenderer::UpdateLightViewBuffers ()
{
    auto frustumVertices = GetGameScene ()->GetActiveCamera ()->GetFrustumVertices (m_width, m_height);
    AABB sceneAABB = GetGameScene ()->GetGameObjectManager ()->GetByName<GameObject> ("root")->GetHierarchyAABB ();

    m_lightBuffers->LoadLightViewMatrices (GetGameScene ()->GetGameObjectManager (), frustumVertices, sceneAABB);
}

size_t GLRenderer::GetPointLightCount () const
{
    return m_lightBuffers->GetPointLightCount ();
}

size_t GLRenderer::GetDirectionalLightCount () const
{
    return m_lightBuffers->GetDirectionalLightCount ();
}

size_t GLRenderer::GetSpotLightCount () const
{
    return m_lightBuffers->GetSpotLightCount ();
}

std::shared_ptr<IFramebuffer> GLRenderer::CreateFramebuffer (unsigned int width, unsigned int height, unsigned int rgbTextureCount, unsigned int rgbaTextureCount, unsigned int depthBufferArrayTextureCount, bool depthStencilRenderbufferEnabled, bool multisampleEnabled)
{
    std::shared_ptr<IFramebuffer> framebuffer;

    if (multisampleEnabled)
    {
        if (GLUtils::GetGLSLVersion ().versionNumber <= 150)
        {
            LogMessage (MSG_LOCATION, EXIT_FAILURE) << "OpenGL 3.2 required for multisample framebuffers";
        }
        else 
        {
            framebuffer = std::make_shared<GLMultisampleFramebuffer> (width, height, rgbTextureCount, rgbaTextureCount, depthBufferArrayTextureCount, depthStencilRenderbufferEnabled);
        }
    }
    else
    {
        framebuffer = std::make_shared<GLFramebuffer> (width, height, rgbTextureCount, rgbaTextureCount, depthBufferArrayTextureCount, depthStencilRenderbufferEnabled);
    }
    
    framebuffer->Initialize ();

    return framebuffer;
}

void GLRenderer::BindDefaultFramebuffer ()
{
    glBindFramebuffer (GL_FRAMEBUFFER, (GLint) 0);
}

void GLRenderer::BindDefaultDepthBuffer ()
{
    glFramebufferTexture (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
}

void GLRenderer::BindDefaultStencilBuffer ()
{
    glFramebufferTexture (GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, 0, 0);
}


void GLRenderer::ClearColorBuffer (const Vector4f& rgba)
{
    glClearColor (rgba[0], rgba[1], rgba[2], rgba[3]);
    glClear (GL_COLOR_BUFFER_BIT);
}

void GLRenderer::ClearDepthBuffer ()
{
    glClearDepth (1.0f);
    glClear (GL_DEPTH_BUFFER_BIT);
}

void GLRenderer::ClearStencilBuffer ()
{
    glClearStencil (0);
    glClear (GL_STENCIL_BUFFER_BIT);
}

void GLRenderer::SetDepthTestEnabled (bool value)
{
    if (value == true)
    {
        glEnable (GL_DEPTH_TEST);
    }
    else
    {   
        glDisable (GL_DEPTH_TEST);
    }    
}

void GLRenderer::SetDepthTestFunction (EDepthTestFunction testFunction)
{
    auto GLFun = [](EDepthTestFunction f)
    {
        switch (f)   
        {
            case EDepthTestFunction::FUNCTION_ALWAYS: return GL_ALWAYS; break;
            case EDepthTestFunction::FUNCTION_EQUAL: return GL_EQUAL; break;
            case EDepthTestFunction::FUNCTION_GEQUAL: return GL_GEQUAL; break;
            case EDepthTestFunction::FUNCTION_GREATER: return GL_GREATER; break;
            case EDepthTestFunction::FUNCTION_LEQUAL: return GL_LEQUAL; break;
            case EDepthTestFunction::FUNCTION_LESS: return GL_LESS; break;
            case EDepthTestFunction::FUNCTION_NEVER: return GL_NEVER; break;
            case EDepthTestFunction::FUNCTION_NOTEQUAL: return GL_NOTEQUAL; break;
            default: return GL_ALWAYS; break;
        }
    };

    glDepthFunc (GLFun (testFunction));
}

void GLRenderer::SetFaceCullingEnabled (bool value)
{
    if (value == true)
    {        
        glEnable (GL_CULL_FACE);
    }
    else
    {   
        glDisable (GL_CULL_FACE);
    }    
}

void GLRenderer::SetFaceCullingMode (EFaceCullingFace face, EFaceCullingDirection direction)
{
    if (face == EFaceCullingFace::FACE_FRONT)
    {
        glCullFace (GL_FRONT);
    }
    else 
    {
        glCullFace (GL_BACK);
    }

    if (direction == EFaceCullingDirection::DIR_CW)
    {        
        glFrontFace (GL_CW);
    }
    else
    {   
        glFrontFace (GL_CCW);
    }    
}

void GLRenderer::SetMultisamplingEnabled (bool value)
{
    if (value == true)
    {        
        glEnable (GL_MULTISAMPLE);
    }
    else
    {   
        glDisable (GL_MULTISAMPLE);
    }    
}

void GLRenderer::SetStencilTestEnabled (bool value)
{
    if (value == true)
    {        
        glEnable (GL_STENCIL_TEST);
        glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
    }
    else
    {   
        glDisable (GL_STENCIL_TEST);
    } 
}

void GLRenderer::SetStencilTestFunction (EStencilTestFunction testFunction, int testValue)
{
    auto GLFun = [](EStencilTestFunction f)
    {
        switch (f)   
        {
            case EStencilTestFunction::FUNCTION_ALWAYS: return GL_ALWAYS; break;
            case EStencilTestFunction::FUNCTION_EQUAL: return GL_EQUAL; break;
            case EStencilTestFunction::FUNCTION_GEQUAL: return GL_GEQUAL; break;
            case EStencilTestFunction::FUNCTION_GREATER: return GL_GREATER; break;
            case EStencilTestFunction::FUNCTION_LEQUAL: return GL_LEQUAL; break;
            case EStencilTestFunction::FUNCTION_LESS: return GL_LESS; break;
            case EStencilTestFunction::FUNCTION_NEVER: return GL_NEVER; break;
            case EStencilTestFunction::FUNCTION_NOTEQUAL: return GL_NOTEQUAL; break;
            default: return GL_ALWAYS; break;
        }
    };

    glStencilFunc (GLFun (testFunction), testValue, 0xff);
    glStencilMask (0xff);
}

void GLRenderer::SetStencilTestOperation (EStencilTestOperation sFail, EStencilTestOperation dpFail, EStencilTestOperation dpPass)
{
    auto GLOp = [](EStencilTestOperation op)
    {
        switch (op)
        {
            case EStencilTestOperation::OP_KEEP: return GL_KEEP; break;
            case EStencilTestOperation::OP_REPLACE: return GL_REPLACE; break;
            case EStencilTestOperation::OP_ZERO: return GL_ZERO; break;
            case EStencilTestOperation::OP_INC: return GL_INCR; break;
            case EStencilTestOperation::OP_DEC: return GL_DECR; break;
            case EStencilTestOperation::OP_INC_WRAP: return GL_INCR_WRAP; break;
            case EStencilTestOperation::OP_DEC_WRAP: return GL_DECR_WRAP; break;
            case EStencilTestOperation::OP_INV: return GL_INVERT; break;
            default: return GL_KEEP; break;
        }
    };

    glStencilOp (GLOp (sFail), GLOp (dpFail), GLOp (dpPass));
}

void GLRenderer::InitializeObjectBuffers ()
{
    // create and load object buffers for all existing objects
    for (auto&& gameObject : GetGameScene ()->GetGameObjectManager ())
    {
        // load buffers for MeshObject only
        if (std::dynamic_pointer_cast<MeshObject> (gameObject) != nullptr)
        {
            gameObject->OnUpdate (*this);
        }      
    }
}

void GLRenderer::InitializeLightUniformBuffers ()
{
    m_lightBuffers->Initialize ();

    // scan objects vector for lights and populate light buffers
    for (auto&& gameObject : GetGameScene ()->GetGameObjectManager ())
    {
        if (std::dynamic_pointer_cast<Light> (gameObject) != nullptr)
        {
            gameObject->OnUpdate (*this);
        }
    }

}

void GLRenderer::DeinitializeLightUniformBuffers ()
{
    m_lightBuffers->Deinitialize ();
}

} // namespace cilantro
