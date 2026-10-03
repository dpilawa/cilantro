#include "graphics/AABBRenderStage.h"
#include "graphics/ShaderProgramNames.h"
#include "graphics/IFramebuffer.h"
#include "scene/GameScene.h"
#include "system/Game.h"

namespace cilantro {

AABBRenderStage::AABBRenderStage (std::shared_ptr<IRenderer> renderer)
    : RenderStage (renderer)
{

}

void AABBRenderStage::Initialize ()
{
    InitializeFramebuffer ();
}

void AABBRenderStage::InitializeFramebuffer ()
{   
    if (m_isFramebufferEnabled)
    {
        m_framebuffer = GetRenderer ()->CreateFramebuffer ({ .width = GetRenderer ()->GetWidth (), .height = GetRenderer ()->GetHeight (), .rgbaTextureCount = 1, .depthStencilRenderbufferEnabled = true, .multisampleEnabled = m_isMultisampleEnabled });
    }
}

void AABBRenderStage::OnFrame ()
{
    RenderStage::OnFrame ();

    // load uniform buffers
    GetRenderer ()->UpdateCameraBuffers (GetRenderer ()->GetGameScene ()->GetActiveCamera ());

    // draw all objects in scene
    GetRenderer ()->DrawAABBGeometryBuffers (GetRenderer ()->GetShaderProgramManager ()->GetByName<IShaderProgram> (ShaderProgramNames::AABB));

    if (m_framebuffer != nullptr)
    {
        m_framebuffer->BlitFramebuffer ();
    }    
}

} // namespace cilantro