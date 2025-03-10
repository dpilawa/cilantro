#include "graphics/ShadowMapRenderStage.h"
#include "graphics/IFramebuffer.h"
#include "graphics/GLShader.h"
#include "graphics/GLShaderProgram.h"
#include "scene/GameScene.h"
#include "system/Game.h"

namespace cilantro {

CShadowMapRenderStage::CShadowMapRenderStage ()
    : CRenderStage ()
{

}

void CShadowMapRenderStage::Initialize ()
{
    InitializeFramebuffer ();

    // set callback for new or modified lights
    CGame::GetMessageBus ().Subscribe<LightUpdateMessage> (
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

void CShadowMapRenderStage::OnFrame ()
{
    CRenderStage::OnFrame ();

    // load uniform buffers
    m_renderer->UpdateLightViewBuffers ();

    // draw all objects in scene
    m_renderer->DrawAllGeometryBuffers (m_renderer->GetShaderProgramManager ().GetByName<IShaderProgram> ("shadowmap_directional_shader"));

    if (m_framebuffer != nullptr)
    {
        m_framebuffer->BlitFramebuffer ();
    }    
}

void CShadowMapRenderStage::InitializeFramebuffer ()
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