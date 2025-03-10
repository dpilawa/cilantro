#include "graphics/ShadowMapRenderStage.h"
#include "graphics/IFramebuffer.h"
#include "graphics/GLShader.h"
#include "graphics/GLShaderProgram.h"
#include "scene/GameScene.h"
#include "system/Game.h"

namespace cilantro {

ShadowMapRenderStage::ShadowMapRenderStage ()
    : RenderStage ()
{

}

void ShadowMapRenderStage::Initialize ()
{
    InitializeFramebuffer ();

    // set callback for new or modified lights
    Game::GetMessageBus ().Subscribe<LightUpdateMessage> (
        [&](const std::shared_ptr<LightUpdateMessage>& message) 
        { 
            if (m_framebuffer != nullptr)
            {
                m_framebuffer->Deinitialize ();
                delete m_framebuffer;
            }

            InitializeFramebuffer ();
        }
    );

}

void ShadowMapRenderStage::OnFrame ()
{
    RenderStage::OnFrame ();

    // load uniform buffers
    m_renderer->UpdateLightViewBuffers ();

    // draw all objects in scene
    m_renderer->DrawAllGeometryBuffers (m_renderer->GetShaderProgramManager ().GetByName<IShaderProgram> ("shadowmap_directional_shader"));

    if (m_framebuffer != nullptr)
    {
        m_framebuffer->BlitFramebuffer ();
    }    
}

void ShadowMapRenderStage::InitializeFramebuffer ()
{   
    if (m_isFramebufferEnabled)
    {
        size_t numLights = m_renderer->GetDirectionalLightCount ();

        if (numLights > 0)
        {
            m_framebuffer = m_renderer->CreateFramebuffer (CILANTRO_SHADOW_MAP_SIZE, CILANTRO_SHADOW_MAP_SIZE, 0, 0, (unsigned int) numLights, false, m_isMultisampleEnabled);
        }
    }
}

} // namespace cilantro