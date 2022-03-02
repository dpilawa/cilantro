#ifndef _GLDEFERREDGEOMETRYRENDERSTAGE_H_
#define _GLDEFERREDGEOMETRYRENDERSTAGE_H_

#include "cilantroengine.h"
#include "graphics/RenderStage.h"

class MeshObject;

class GLDeferredGeometryRenderStage : public RenderStage
{
public:
    __EAPI GLDeferredGeometryRenderStage ();
    __EAPI virtual ~GLDeferredGeometryRenderStage ();

    __EAPI virtual void Initialize ();
    __EAPI virtual void Deinitialize ();

    virtual void OnFrame ();

protected:

    virtual void InitializeFramebuffer ();



};

#endif