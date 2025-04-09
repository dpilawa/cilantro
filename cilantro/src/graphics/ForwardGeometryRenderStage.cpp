#include "graphics/ForwardGeometryRenderStage.h"
#include "graphics/IFramebuffer.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/MeshObject.h"
#include <string>

namespace cilantro {

ForwardGeometryRenderStage::ForwardGeometryRenderStage (std::shared_ptr<IRenderer> renderer) 
    : RenderStage (renderer)
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
        m_framebuffer = GetRenderer ()->CreateFramebuffer (GetRenderer ()->GetWidth (), GetRenderer ()->GetHeight (), 0, 1, 0, true, m_isMultisampleEnabled);
    }
}

void ForwardGeometryRenderStage::OnFrame ()
{
    RenderStage::OnFrame ();

    // load uniform buffers
    GetRenderer ()->UpdateCameraBuffers (GetRenderer ()->GetGameScene ()->GetActiveCamera ());

    // draw all objects in scene
    for (auto gameObject : GetRenderer ()->GetGameScene ()->GetGameObjectManager ())
    {
        gameObject->OnDraw (*(m_renderer.lock ()));
    }

    if (m_framebuffer != nullptr)
    {
        m_framebuffer->BlitFramebuffer ();
    }
}


} // namespace cilantro