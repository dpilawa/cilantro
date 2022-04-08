#pragma once

#include "cilantroengine.h"
#include "graphics/RenderStage.h"

class CShadowMapRenderStage : public CRenderStage
{
public:
    CShadowMapRenderStage ();
    virtual ~CShadowMapRenderStage () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual void Initialize () override;
    virtual void Deinitialize () override {};

    virtual void OnFrame () override;

    ///////////////////////////////////////////////////////////////////////////

protected:
    virtual void InitializeFramebuffer () override;
};