#pragma once

#include "cilantroengine.h"
#include "graphics/RenderStage.h"

namespace cilantro {

class MeshObject;

class __CEAPI DeferredGeometryRenderStage : public RenderStage
{
public:
    __EAPI DeferredGeometryRenderStage (std::shared_ptr<IRenderer> renderer);
    __EAPI virtual ~DeferredGeometryRenderStage ();

    ///////////////////////////////////////////////////////////////////////////

    __EAPI virtual void Initialize () override;
    __EAPI virtual void Deinitialize () override;

    __EAPI virtual void InitializeFramebuffer () override;

    __EAPI virtual void OnFrame () override;

    ///////////////////////////////////////////////////////////////////////////

};

} // namespace cilantro

