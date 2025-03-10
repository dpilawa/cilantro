#pragma once

#include "cilantroengine.h"
#include "graphics/RenderStage.h"

namespace cilantro {

class MeshObject;

class CDeferredGeometryRenderStage : public CRenderStage
{
public:
    __EAPI CDeferredGeometryRenderStage ();
    __EAPI virtual ~CDeferredGeometryRenderStage () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual void Initialize () override;
    virtual void Deinitialize () override {};

    virtual void OnFrame () override;

    ///////////////////////////////////////////////////////////////////////////

protected:
    virtual void InitializeFramebuffer () override;
};

} // namespace cilantro

