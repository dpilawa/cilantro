#pragma once

#include "cilantroengine.h"
#include "graphics/RenderStage.h"

class MeshObject;

class CGLDeferredGeometryRenderStage : public CRenderStage
{
public:
    __EAPI CGLDeferredGeometryRenderStage ();
    __EAPI virtual ~CGLDeferredGeometryRenderStage () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual void Initialize () override;
    virtual void Deinitialize () override {};

    virtual void OnFrame () override;

    ///////////////////////////////////////////////////////////////////////////

protected:
    virtual void InitializeFramebuffer ();
};

