#include "graphics/DeferredLightingRenderStage.h"
#include "graphics/IRenderer.h"
#include "graphics/IFramebuffer.h"
#include "graphics/ShaderProgram.h"

namespace cilantro {

DeferredLightingRenderStage::DeferredLightingRenderStage (std::shared_ptr<IRenderer> renderer)
    : SurfaceRenderStage (renderer)
{
}

DeferredLightingRenderStage::~DeferredLightingRenderStage ()
{
}

void DeferredLightingRenderStage::OnFrame ()
{
    m_shaderProgram->Use ();

    // set shadow map uniform (if shadow mapping is enabled)
    m_shaderProgram->SetUniformInt ("shadowMapEnabled", GetRenderer ()->IsShadowMapping () ? 1 : 0);

    // bind shadow maps (if exist)
    if (m_linkedDepthTextureArrayFramebuffer != nullptr && m_linkedDepthTextureArrayFramebuffer->IsDepthTextureArrayEnabled ())
    {
        m_linkedDepthTextureArrayFramebuffer->BindFramebufferDepthTextureArrayAsColor (CILANTRO_SHADOW_MAP_BINDING);
    }

    SurfaceRenderStage::OnFrame ();
}

} // namespace cilantro