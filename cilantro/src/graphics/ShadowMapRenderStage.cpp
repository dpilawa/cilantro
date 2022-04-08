#include "graphics/ShadowMapRenderStage.h"
#include "graphics/IFramebuffer.h"
#include "scene/GameScene.h"

CShadowMapRenderStage::CShadowMapRenderStage ()
    : CRenderStage ()
{

}

void CShadowMapRenderStage::Initialize ()
{
    InitializeFramebuffer ();
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
        m_framebuffer = m_renderer->CreateFramebuffer (CILANTRO_SHADOW_MAP_SIZE, CILANTRO_SHADOW_MAP_SIZE, 0, 0, numLights == 0 ? 1 : numLights , false, m_isMultisampleEnabled);
    }
}