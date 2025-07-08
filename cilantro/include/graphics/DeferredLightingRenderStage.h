#pragma once

#include "cilantroengine.h"
#include "graphics/SurfaceRenderStage.h"

namespace cilantro {

class __CEAPI DeferredLightingRenderStage : public SurfaceRenderStage
{
public:
    __EAPI DeferredLightingRenderStage (std::shared_ptr<IRenderer> renderer);
    __EAPI virtual ~DeferredLightingRenderStage ();

    __EAPI virtual void OnFrame () override;

};

} // namespace cilantro