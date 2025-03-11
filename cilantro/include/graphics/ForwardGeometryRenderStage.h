#pragma once

#include "cilantroengine.h"
#include "graphics/RenderStage.h"

namespace cilantro {

class MeshObject;

class __CEAPI ForwardGeometryRenderStage : public RenderStage
{
public:
    __EAPI ForwardGeometryRenderStage ();
    __EAPI virtual ~ForwardGeometryRenderStage ();

    ///////////////////////////////////////////////////////////////////////////

    __EAPI virtual void Initialize () override;
    __EAPI virtual void Deinitialize () override;

    __EAPI virtual void InitializeFramebuffer () override;

    __EAPI virtual void OnFrame () override;

    ///////////////////////////////////////////////////////////////////////////
    
};

} // namespace cilantro

