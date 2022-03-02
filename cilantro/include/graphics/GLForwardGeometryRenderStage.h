#ifndef _GLFORWARDGEOMETRYRENDERSTAGE_H_
#define _GLFORWARDGEOMETRYRENDERSTAGE_H_

#include "cilantroengine.h"
#include "graphics/RenderStage.h"

class MeshObject;

class GLForwardGeometryRenderStage : public RenderStage
{
public:
    __EAPI GLForwardGeometryRenderStage ();
    __EAPI virtual ~GLForwardGeometryRenderStage ();

    __EAPI virtual void Initialize ();
    __EAPI virtual void Deinitialize ();

    virtual void OnFrame ();

protected:

    virtual void InitializeFramebuffer ();
    


};

#endif