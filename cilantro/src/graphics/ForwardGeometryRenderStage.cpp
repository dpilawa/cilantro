#include "graphics/ForwardGeometryRenderStage.h"
#include "graphics/IFramebuffer.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/MeshObject.h"
#include <string>

namespace cilantro {

ForwardGeometryRenderStage::ForwardGeometryRenderStage () 
    : RenderStage ()
{
}

ForwardGeometryRenderStage::~ForwardGeometryRenderStage ()
{
}

void ForwardGeometryRenderStage::Initialize ()
{    
    InitializeFramebuffer ();
}

void ForwardGeometryRenderStage::Deinitialize()
{
}

void ForwardGeometryRenderStage::InitializeFramebuffer ()
{
    if (m_isFramebufferEnabled)
    {
        m_framebuffer = m_renderer->CreateFramebuffer (m_renderer->GetWidth (), m_renderer->GetHeight (), 0, 1, 0, true, m_isMultisampleEnabled);
    }
}

void ForwardGeometryRenderStage::OnFrame ()
{
    RenderStage::OnFrame ();

    // load uniform buffers
    m_renderer->UpdateCameraBuffers (m_renderer->GetGameScene ()->GetActiveCamera ());

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


} // namespace cilantro