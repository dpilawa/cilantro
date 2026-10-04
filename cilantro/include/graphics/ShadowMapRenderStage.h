#pragma once

#include "cilantroengine.h"
#include "graphics/RenderStage.h"
#include "system/MessageBus.h"

namespace cilantro {

class __CEAPI ShadowMapRenderStage : public RenderStage
{
public:
    __EAPI ShadowMapRenderStage (std::shared_ptr<IRenderer> renderer);
    __EAPI virtual ~ShadowMapRenderStage () {};

    ///////////////////////////////////////////////////////////////////////////

    __EAPI virtual void Initialize () override;
    __EAPI virtual void Deinitialize () override;

    __EAPI virtual void InitializeFramebuffer () override;

    __EAPI virtual void OnFrame () override;

    ///////////////////////////////////////////////////////////////////////////

private:
    // subscription to light updates (recreates shadow map framebuffer), released on deinitialization
    MessageBus::ScopedSubscription m_lightSubscription;

};

} // namespace cilantro
