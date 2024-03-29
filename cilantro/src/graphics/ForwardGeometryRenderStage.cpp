#include "graphics/ForwardGeometryRenderStage.h"
#include "graphics/IFramebuffer.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/MeshObject.h"
#include <string>

CForwardGeometryRenderStage::CForwardGeometryRenderStage () 
    : CRenderStage ()
{
}

void CForwardGeometryRenderStage::Initialize ()
{    
    InitializeFramebuffer ();
}

void CForwardGeometryRenderStage::OnFrame ()
{
    CRenderStage::OnFrame ();

    // load uniform buffers
    m_renderer->UpdateCameraBuffers (*m_renderer->GetGameScene ()->GetActiveCamera ());

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

void CForwardGeometryRenderStage::InitializeFramebuffer ()
{
    if (m_isFramebufferEnabled)
    {
        m_framebuffer = m_renderer->CreateFramebuffer (m_renderer->GetWidth (), m_renderer->GetHeight (), 0, 1, 0, true, m_isMultisampleEnabled);
    }
}

