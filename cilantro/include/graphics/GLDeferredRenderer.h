#ifndef _GLDEFERREDRENDERER_H_
#define _GLDEFERREDRENDERER_H_

#include "cilantroengine.h"
#include "graphics/GLRenderer.h"

class GLPostprocess;

class GLDeferredRenderer : public GLRenderer
{
public:
    __EAPI GLDeferredRenderer (unsigned int width, unsigned int height);
    __EAPI virtual ~GLDeferredRenderer ();

    __EAPI virtual void Initialize ();

    __EAPI virtual void RenderFrame ();

    __EAPI virtual Framebuffer* GetCurrentFramebuffer () const;

private:

    GLPostprocess* lightingPass;

    virtual ShaderProgram& GetMeshObjectGeometryShaderProgram (const MeshObject& meshObject);
    virtual ShaderProgram& GetMeshObjectLightingShaderProgram (const MeshObject& meshObject);

};

#endif