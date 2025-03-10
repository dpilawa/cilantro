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

    virtual void Initialize () override;
    virtual void Deinitialize () override {};

    virtual void OnFrame () override;

    ///////////////////////////////////////////////////////////////////////////

protected:
    virtual void InitializeFramebuffer () override;
};

} // namespace cilantro

