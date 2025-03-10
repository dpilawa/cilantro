#pragma once

#include "cilantroengine.h"
#include "graphics/RenderStage.h"

namespace cilantro {

class MeshObject;

class DeferredGeometryRenderStage : public RenderStage
{
public:
    __EAPI DeferredGeometryRenderStage ();
    __EAPI virtual ~DeferredGeometryRenderStage () {};

    ///////////////////////////////////////////////////////////////////////////

    __EAPI virtual void Initialize () override;
    __EAPI virtual void Deinitialize () override {};

    __EAPI virtual void InitializeFramebuffer () override;

    __EAPI virtual void OnFrame () override;

    ///////////////////////////////////////////////////////////////////////////

};

} // namespace cilantro

