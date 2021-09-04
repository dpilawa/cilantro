#include "system/EngineContext.h"
#include "graphics/GLDeferredRenderer.h"
#include "graphics/GLPostprocess.h"
#include "scene/MeshObject.h"
#include <set>

GLDeferredRenderer::GLDeferredRenderer (unsigned int width, unsigned int height) : GLRenderer (width, height)
{
    // g-buffer
    framebuffer = new GLFramebuffer (width, height, 0, 5);

    lightingShaderStagesCount = 0;
}

GLDeferredRenderer::~GLDeferredRenderer ()
{

}

void GLDeferredRenderer::Initialize ()
{
    GLRenderer::Initialize ();  
    
    // initialize framebuffers
    framebuffer->Initialize ();
}

void GLDeferredRenderer::RenderFrame ()
{
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

    // base class function - will include also added postprocess(es) for lightning pass(es)
    Renderer::RenderFrame ();

    // check for errors
    CheckGLError (MSG_LOCATION);    
}

void GLDeferredRenderer::Update (Material& material)
{
    handle_t shaderProgramHandle = material.GetDeferredLightingPassShaderProgram ().GetHandle ();
    std::string shaderProgramName = this->GetShaderProgramManager ().GetByHandle<ShaderProgram> (shaderProgramHandle).GetName ();

    // add material's shader program to set of used shader programs handles
    // add lighting deferred pass postprocesses for each program
    if (lightingShaders.find (shaderProgramHandle) == lightingShaders.end ())
    {
        lightingShaderStagesCount++;
        lightingShaders.insert (shaderProgramHandle);
        GLPostprocess& p = this->AddPostprocess <GLPostprocess> ("deferredLightingStage_" + shaderProgramName);
        p.SetShaderProgram (shaderProgramName);
        p.SetStencilTestEnabled (true).SetStencilTest (StencilTestFunction::FUNCTION_EQUAL, shaderProgramHandle);
        p.SetClearOnFrameEnabled (true);
        p.SetPipelineFramebufferInputLink (PipelineLink::LINK_BASE);
        p.SetPipelineRenderbufferLink (PipelineLink::LINK_BASE);
        p.SetPipelineFramebufferOutputLink (PipelineLink::LINK_FIRST);

        // cycle postprocess sequence so that newly addded postprocess is in front
        // simple vector rotation to the right
        std::rotate (postprocessPipeline.rbegin (), postprocessPipeline.rbegin () + 1, postprocessPipeline.rend ());

        // update flags of other deferred lighting stages (if present)
        if (lightingShaderStagesCount > 1)
        {
            handle_t stageHandle = postprocessPipeline[1];

            GLPostprocess& stage = GetPostprocessManager ().GetByHandle<GLPostprocess> (stageHandle);
            stage.SetClearOnFrameEnabled (false);
        }

        // update pipeline links of 1st stage following deferred lighting stages
        if (postprocessPipeline.size () > lightingShaderStagesCount)
        {
            handle_t stageHandle = postprocessPipeline[lightingShaderStagesCount];

            GLPostprocess& stage = GetPostprocessManager ().GetByHandle<GLPostprocess> (stageHandle);
            stage.SetClearOnFrameEnabled (true);
            stage.SetPipelineFramebufferInputLink (PipelineLink::LINK_FIRST);
            stage.SetPipelineRenderbufferLink (PipelineLink::LINK_CURRENT);
            stage.SetPipelineFramebufferOutputLink (PipelineLink::LINK_CURRENT);
        }
    }

    GLRenderer::Update (material);
}

ShaderProgram& GLDeferredRenderer::GetMeshObjectGeometryShaderProgram (const MeshObject& meshObject) 
{
    return meshObject.GetMaterial ().GetDeferredGeometryPassShaderProgram ();
}

ShaderProgram& GLDeferredRenderer::GetMeshObjectLightingShaderProgram (const MeshObject& meshObject) 
{
    return meshObject.GetMaterial ().GetDeferredLightingPassShaderProgram ();
}