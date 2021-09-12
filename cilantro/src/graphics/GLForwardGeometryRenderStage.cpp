#include "system/EngineContext.h"
#include "graphics/GLGeometryRenderStage.h"
#include "graphics/GLForwardGeometryRenderStage.h"
#include "scene/MeshObject.h"

GLForwardGeometryRenderStage::GLForwardGeometryRenderStage () : GLGeometryRenderStage ()
{
}

GLForwardGeometryRenderStage::~GLForwardGeometryRenderStage ()
{

}

void GLForwardGeometryRenderStage::Initialize ()
{    
    GLGeometryRenderStage::Initialize ();
    
    // initialize framebuffer
    if (multisampleEnabled)
    {
#if (CILANTRO_GL_VERSION <= 140)
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "OpenGL 3.2 required for multisample framebuffers";
#else
        framebuffer = new GLMultisampleFramebuffer (EngineContext::GetRenderer ().GetWidth (), EngineContext::GetRenderer ().GetHeight (), 0, 1);
#endif
    }
    else
    {
        framebuffer = new GLFramebuffer (EngineContext::GetRenderer ().GetWidth (), EngineContext::GetRenderer ().GetHeight (), 0, 1);
    }    

    framebuffer->Initialize ();

}

void GLForwardGeometryRenderStage::Deinitialize ()
{
    framebuffer->Deinitialize ();
    delete framebuffer;

    GLGeometryRenderStage::Deinitialize ();
}

void GLForwardGeometryRenderStage::OnFrame ()
{
    GLRenderStage::OnFrame ();

    // bind framebuffer
    framebuffer->BindFramebuffer ();

    // load uniform buffers
    LoadMatrixUniformBuffers ();

    // set viewport
    glViewport (0, 0, this->GetFramebuffer ()->GetWidth (), this->GetFramebuffer ()->GetHeight ());

    // draw all objects in scene
    for (auto gameObject : EngineContext::GetGameScene ().GetGameObjectManager ())
    {
        gameObject->OnDraw (*this);
    }

#if (CILANTRO_GL_VERSION > 140)
    // blit framebuffer
    if (multisampleEnabled)
    {
        static_cast<GLMultisampleFramebuffer*>(framebuffer)->BlitFramebuffer ();
    }
#endif

    // unbind framebuffer
    framebuffer->UnbindFramebuffer ();

    // check for errors
    CheckGLError (MSG_LOCATION);
}

ShaderProgram& GLForwardGeometryRenderStage::GetMeshObjectGeometryShaderProgram (const MeshObject& meshObject) 
{
    return meshObject.GetMaterial ().GetForwardShaderProgram ();
}

ShaderProgram& GLForwardGeometryRenderStage::GetMeshObjectLightingShaderProgram (const MeshObject& meshObject) 
{
    return meshObject.GetMaterial ().GetForwardShaderProgram ();
}