#pragma once

#include "cilantroengine.h"
#include "graphics/RenderStage.h"

namespace cilantro {

class MeshObject;

class CForwardGeometryRenderStage : public CRenderStage
{
public:
    __EAPI CForwardGeometryRenderStage ();
    __EAPI virtual ~CForwardGeometryRenderStage () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual void Initialize () override;
    virtual void Deinitialize () override {};

    virtual void OnFrame () override;

    ///////////////////////////////////////////////////////////////////////////

protected:
    virtual void InitializeFramebuffer () override;
    
};

} // namespace cilantro

