#include "system/EngineContext.h"
#include "graphics/GLDeferredGeometryRenderStage.h"
#include "graphics/GLQuadRenderStage.h"
#include "graphics/GLRenderStage.h"
#include "scene/MeshObject.h"
#include <set>

GLDeferredGeometryRenderStage::GLDeferredGeometryRenderStage () : GLGeometryRenderStage ()
{
    lightingShaderStagesCount = 0;
}

GLDeferredGeometryRenderStage::~GLDeferredGeometryRenderStage ()
{

}

void GLDeferredGeometryRenderStage::Initialize ()
{
    GLGeometryRenderStage::Initialize ();  

    // initialize framebuffers (g-buffer)
    // any multisample flags ignored here
    framebuffer = new GLFramebuffer (EngineContext::GetRenderer ().GetWidth (), EngineContext::GetRenderer ().GetHeight (), 0, 5);
    framebuffer->Initialize ();
}

void GLDeferredGeometryRenderStage::Deinitialize ()
{    
    // deinitialize framebuffers
    framebuffer->Deinitialize ();
    delete framebuffer;

    GLGeometryRenderStage::Deinitialize ();  
}

void GLDeferredGeometryRenderStage::OnFrame ()
{
    GLRenderStage::OnFrame ();

    // bind g-buffer
    framebuffer->BindFramebuffer ();

    // clear frame, depth and stencil buffers
    glClearColor (0.0f, 0.0f, 0.0f, 1.0f);   
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // set viewport
    glViewport (0, 0, this->GetFramebuffer ()->GetWidth (), this->GetFramebuffer ()->GetHeight ());

    // enable depth and stencil buffer testing
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_STENCIL_TEST);

    // load uniform buffers
    LoadMatrixUniformBuffers ();

    // GEOMETRY PASS
    // draw all objects in scene using geometry shader, construct g-buffer
    // set stencil value to lowest depth lighting shader handle

    glStencilOp (GL_KEEP, GL_KEEP, GL_REPLACE);

    for (auto gameObject : EngineContext::GetGameScene ().GetGameObjectManager ())
    {
        // overwrite stencil value with material Id
        if (MeshObject* meshObject = dynamic_cast<MeshObject*>(gameObject.get ()))
        {
            glStencilFunc (GL_ALWAYS, meshObject->GetMaterial ().GetDeferredLightingPassShaderProgram ().GetHandle (), 0xff);
            glStencilMask (0xff);
        }

        // draw to g-buffer
        gameObject->OnDraw (*this);

    }

    // unbind framebuffer
    framebuffer->UnbindFramebuffer ();

    // check for errors
    CheckGLError (MSG_LOCATION);    
}

void GLDeferredGeometryRenderStage::Update (Material& material)
{
    handle_t shaderProgramHandle = material.GetDeferredLightingPassShaderProgram ().GetHandle ();
    std::string shaderProgramName = EngineContext::GetRenderer().GetShaderProgramManager ().GetByHandle<ShaderProgram> (shaderProgramHandle).GetName ();

    // add material's shader program to set of used shader programs handles
    // add lighting deferred pass renderStages for each program
    if (lightingShaders.find (shaderProgramHandle) == lightingShaders.end ())
    {
        // first rotate the pipeline to the left so that geometry stage is last
        EngineContext::GetRenderer ().RotateRenderPipelineLeft ();

        lightingShaderStagesCount++;
        lightingShaders.insert (shaderProgramHandle);
        GLQuadRenderStage& p = EngineContext::GetRenderer ().AddRenderStage <GLQuadRenderStage> ("deferredLightingStage_" + shaderProgramName);
        p.SetShaderProgram (shaderProgramName);
        p.SetStencilTestEnabled (true).SetStencilTest (StencilTestFunction::FUNCTION_EQUAL, shaderProgramHandle);
        p.SetClearOnFrameEnabled (true);
        p.SetDepthTestEnabled (false);
        p.SetPipelineFramebufferInputLink (PipelineLink::LINK_FIRST);
        p.SetPipelineRenderbufferLink (PipelineLink::LINK_FIRST);
        p.SetPipelineFramebufferDrawLink (PipelineLink::LINK_SECOND);

        // rotate pipeline twice to the right, so that ultimately geometry stage is first and newly added stage is second
        EngineContext::GetRenderer ().RotateRenderPipelineRight ();
        EngineContext::GetRenderer ().RotateRenderPipelineRight ();
        
        // update flags of other deferred lighting stages (if present)
        if (lightingShaderStagesCount > 1)
        {
            handle_t stageHandle = EngineContext::GetRenderer ().GetRenderPipeline ()[2];

            GLQuadRenderStage& stage = EngineContext::GetRenderer ().GetRenderStageManager ().GetByHandle<GLQuadRenderStage> (stageHandle);
            stage.SetClearOnFrameEnabled (false);
        }

        // update pipeline links of 1st stage following deferred lighting stages
        if (EngineContext::GetRenderer ().GetRenderPipeline ().size () > lightingShaderStagesCount + 1)
        {
            handle_t stageHandle = EngineContext::GetRenderer ().GetRenderPipeline ()[lightingShaderStagesCount + 1];

            GLRenderStage& stage = EngineContext::GetRenderer ().GetRenderStageManager ().GetByHandle<GLRenderStage> (stageHandle);
            stage.SetPipelineFramebufferInputLink (PipelineLink::LINK_SECOND);
            stage.SetPipelineRenderbufferLink (PipelineLink::LINK_CURRENT);
            stage.SetPipelineFramebufferDrawLink (PipelineLink::LINK_CURRENT);
        }
    }

    GLGeometryRenderStage::Update (material);
}

ShaderProgram& GLDeferredGeometryRenderStage::GetMeshObjectGeometryShaderProgram (const MeshObject& meshObject) 
{
    return meshObject.GetMaterial ().GetDeferredGeometryPassShaderProgram ();
}

ShaderProgram& GLDeferredGeometryRenderStage::GetMeshObjectLightingShaderProgram (const MeshObject& meshObject) 
{
    return meshObject.GetMaterial ().GetDeferredLightingPassShaderProgram ();
}