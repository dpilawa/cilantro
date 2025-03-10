#pragma once

#include "cilantroengine.h"
#include "graphics/RenderStage.h"

namespace cilantro {

class MeshObject;

class ForwardGeometryRenderStage : public RenderStage
{
public:
    __EAPI ForwardGeometryRenderStage ();
    __EAPI virtual ~ForwardGeometryRenderStage () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual void Initialize () override;
    virtual void Deinitialize () override {};

    virtual void OnFrame () override;

    ///////////////////////////////////////////////////////////////////////////

protected:
    virtual void InitializeFramebuffer () override;
    
};

} // namespace cilantro

