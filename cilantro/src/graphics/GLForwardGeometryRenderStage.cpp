#include "graphics/GLForwardGeometryRenderStage.h"
#include "graphics/IFramebuffer.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/MeshObject.h"
#include "glad/glad.h"
#include <string>

CGLForwardGeometryRenderStage::CGLForwardGeometryRenderStage () 
    : CRenderStage ()
{
}

void CGLForwardGeometryRenderStage::Initialize ()
{    
    InitializeFramebuffer ();
}

void CGLForwardGeometryRenderStage::OnFrame ()
{
    CRenderStage::OnFrame ();

    // load uniform buffers
    m_renderer->UpdateCameraBuffers (*m_renderer->GetGameScene ()->GetActiveCamera ());

    // set viewport
    glViewport (0, 0, m_renderer->GetWidth (), m_renderer->GetHeight ());

    // draw all objects in scene
    for (auto gameObject : m_renderer->GetGameScene ()->GetGameObjectManager ())
    {
        gameObject->OnDraw (*m_renderer);
    }

    if (m_framebuffer != nullptr)
    {
        m_framebuffer->BlitFramebuffer ();
    }
}

void CGLForwardGeometryRenderStage::InitializeFramebuffer ()
{
    if (m_isFramebufferEnabled)
    {
        m_framebuffer = m_renderer->CreateFramebuffer (0, 1, m_isMultisampleEnabled);
    }
}

