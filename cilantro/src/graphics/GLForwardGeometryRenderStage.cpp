#include "graphics/GLForwardGeometryRenderStage.h"
#include "graphics/RenderStage.h"
#include "graphics/Framebuffer.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/MeshObject.h"
#include "glad/glad.h"
#include <string>

GLForwardGeometryRenderStage::GLForwardGeometryRenderStage () : RenderStage ()
{
}

GLForwardGeometryRenderStage::~GLForwardGeometryRenderStage ()
{

}

void GLForwardGeometryRenderStage::Initialize ()
{    
    InitializeFramebuffer ();
}

void GLForwardGeometryRenderStage::Deinitialize ()
{
}

void GLForwardGeometryRenderStage::OnFrame ()
{
    RenderStage::OnFrame ();

    // load uniform buffers
    renderer->UpdateCameraBuffers (*renderer->GetGameScene ()->GetActiveCamera ());

    // set viewport
    glViewport (0, 0, renderer->GetWidth (), renderer->GetHeight ());

    // draw all objects in scene
    for (auto gameObject : renderer->GetGameScene ()->GetGameObjectManager ())
    {
        gameObject->OnDraw (*renderer);
    }

    if (framebuffer != nullptr)
    {
        framebuffer->BlitFramebuffer ();
    }
}

void GLForwardGeometryRenderStage::InitializeFramebuffer ()
{
    if (framebufferEnabled)
    {
        framebuffer = renderer->CreateFramebuffer (0, 1, multisampleEnabled);
    }
}

