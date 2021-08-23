#include "system/EngineContext.h"
#include "graphics/GLDeferredRenderer.h"
#include "graphics/GLPostprocess.h"
#include "scene/MeshObject.h"
#include <set>

GLDeferredRenderer::GLDeferredRenderer (unsigned int width, unsigned int height) : GLRenderer (width, height)
{
    // g-buffer
    framebuffer = new GLFramebuffer (width, height, 0, 5);

    // lighting pass postprocess
    lightingPass = new GLPostprocess ();
    lightingPass->SetClearOnFrameEnabled (false);
    lightingPass->SetStencilTestEnabled (true);
}

GLDeferredRenderer::~GLDeferredRenderer ()
{

}

void GLDeferredRenderer::Initialize ()
{
    GLRenderer::Initialize ();  
    
    // initialize framebuffers
    framebuffer->Initialize ();
    lightingPass->Initialize ();
}

void GLDeferredRenderer::RenderFrame ()
{
    // set of distinct lighting pass shaders used in the frame
    std::set<unsigned int> lightingShaders;

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
            lightingShaders.insert (meshObject->GetMaterial ().GetDeferredLightingPassShaderProgram ().GetHandle ());
            glStencilFunc (GL_ALWAYS, meshObject->GetMaterial ().GetDeferredLightingPassShaderProgram ().GetHandle (), 0xff);
            glStencilMask (0xff);
        }

        // draw to g-buffer
        gameObject->OnDraw (*this);

    }

    // unbind framebuffer
    framebuffer->UnbindFramebuffer ();

    // LIGHTING PASS
    // for each relevant lighting shader, execute lightning fragment shader using stencil test
    // each stage us using a post-process
    
    postprocessStage = 0;    
    lightingPass->SetClearOnFrameEnabled (true);
    
    for (unsigned int lightingShaderHandle : lightingShaders)
    {
        // use shader
        lightingPass->SetShaderProgram (shaderPrograms.GetByHandle<GLShaderProgram> (lightingShaderHandle).GetName ());

        // set stencil test parameters
        lightingPass->SetStencilTest (StencilTestFunction::FUNCTION_EQUAL, lightingShaderHandle);

        // render
        lightingPass->OnFrame ();

        // do not clear frame on next draw
        lightingPass->SetClearOnFrameEnabled (false);
    }

    // base class function
    postprocessStage = 1;
    Renderer::RenderFrame ();

    // check for errors
    CheckGLError (MSG_LOCATION);    
}

Framebuffer* GLDeferredRenderer::GetCurrentFramebuffer () const
{
    if (postprocessStage == 0) 
    {
        return GetFramebuffer ();
    }
    else if (postprocessStage == 1) 
    {
        return lightingPass->GetFramebuffer ();
    }
    else 
    {
        return (*(postprocesses.begin() + (postprocessStage - 2))).get()->GetFramebuffer ();
    }
}

ShaderProgram& GLDeferredRenderer::GetMeshObjectGeometryShaderProgram (const MeshObject& meshObject) 
{
    return meshObject.GetMaterial ().GetDeferredGeometryPassShaderProgram ();
}

ShaderProgram& GLDeferredRenderer::GetMeshObjectLightingShaderProgram (const MeshObject& meshObject) 
{
    return meshObject.GetMaterial ().GetDeferredLightingPassShaderProgram ();
}