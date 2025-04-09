#include "graphics/ShadowMapRenderStage.h"
#include "graphics/IFramebuffer.h"
#include "graphics/GLShader.h"
#include "graphics/GLShaderProgram.h"
#include "scene/GameScene.h"
#include "system/Game.h"

namespace cilantro {

ShadowMapRenderStage::ShadowMapRenderStage (std::shared_ptr<IRenderer> renderer)
    : RenderStage (renderer)
{

}

void ShadowMapRenderStage::Initialize ()
{
    InitializeFramebuffer ();

    // set callback for new or modified lights
    GetRenderer ()->GetGameScene ()->GetGame ()->GetMessageBus ()->Subscribe<LightUpdateMessage> (
        [&](const std::shared_ptr<LightUpdateMessage>& message) 
        { 
            if (m_framebuffer != nullptr)
            {
                m_framebuffer->Deinitialize ();
            }

            InitializeFramebuffer ();
        }
    );

}

void ShadowMapRenderStage::InitializeFramebuffer ()
{   
    if (m_isFramebufferEnabled)
    {
        size_t numLights = GetRenderer ()->GetDirectionalLightCount ();

        if (numLights > 0)
        {
            m_framebuffer = GetRenderer ()->CreateFramebuffer (CILANTRO_SHADOW_MAP_SIZE, CILANTRO_SHADOW_MAP_SIZE, 0, 0, (unsigned int) numLights, false, m_isMultisampleEnabled);
        }
    }
}

void ShadowMapRenderStage::OnFrame ()
{
    RenderStage::OnFrame ();

    // load uniform buffers
    GetRenderer ()->UpdateLightViewBuffers ();

    // draw all objects in scene
    GetRenderer ()->DrawAllGeometryBuffers (GetRenderer ()->GetShaderProgramManager ()->GetByName<IShaderProgram> ("shadowmap_directional_shader"));

    if (m_framebuffer != nullptr)
    {
        m_framebuffer->BlitFramebuffer ();
    }    
}

} // namespace cilantro