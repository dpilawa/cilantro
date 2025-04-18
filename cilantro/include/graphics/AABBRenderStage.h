#pragma once

#include "cilantroengine.h"
#include "graphics/RenderStage.h"

namespace cilantro {

class __CEAPI AABBRenderStage : public RenderStage
{
public:
    __EAPI AABBRenderStage (std::shared_ptr<IRenderer> renderer);
    __EAPI virtual ~AABBRenderStage () {};

    ///////////////////////////////////////////////////////////////////////////

    __EAPI virtual void Initialize () override;
    __EAPI virtual void Deinitialize () override {};

    __EAPI virtual void InitializeFramebuffer () override;

    __EAPI virtual void OnFrame () override;

    ///////////////////////////////////////////////////////////////////////////
    
};

} // namespace cilantro