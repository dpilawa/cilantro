#include "system/Game.h"
#include "graphics/GLGeometryRenderStage.h"
#include "graphics/GLForwardGeometryRenderStage.h"
#include "graphics/GLFramebuffer.h"
#if (CILANTRO_GL_VERSION > 140)
#include "graphics/GLMultisampleFramebuffer.h"
#endif
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
    
    InitializeFramebuffer ();
}

void GLForwardGeometryRenderStage::Deinitialize ()
{
    GLGeometryRenderStage::Deinitialize ();
}

void GLForwardGeometryRenderStage::OnFrame ()
{
    GLGeometryRenderStage::OnFrame ();

    // load uniform buffers
    LoadMatrixUniformBuffers ();

    // set viewport
    glViewport (0, 0, renderer->GetWidth (), renderer->GetHeight ());

    // draw all objects in scene
    for (auto gameObject : renderer->GetGameScene ()->GetGameObjectManager ())
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

    // check for errors
    CheckGLError (MSG_LOCATION);
}

void GLForwardGeometryRenderStage::InitializeFramebuffer ()
{
    if (framebufferEnabled)
    {
        framebuffer = renderer->CreateFramebuffer (0, 1, multisampleEnabled);
    }
}

std::string GLForwardGeometryRenderStage::GetMeshObjectGeometryShaderProgram (const MeshObject& meshObject) 
{
    return meshObject.GetMaterial ().GetForwardShaderProgram ();
}

std::string GLForwardGeometryRenderStage::GetMeshObjectLightingShaderProgram (const MeshObject& meshObject) 
{
    return meshObject.GetMaterial ().GetForwardShaderProgram ();
}