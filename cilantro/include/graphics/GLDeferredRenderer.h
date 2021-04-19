#ifndef _GLDEFERREDRENDERER_H_
#define _GLDEFERREDRENDERER_H_

#include "cilantroengine.h"
#include "graphics/GLRenderer.h"

class GLDeferredRenderer : public GLRenderer
{
public:
    __EAPI GLDeferredRenderer (unsigned int width, unsigned int height);
    __EAPI virtual ~GLDeferredRenderer ();

    __EAPI virtual void Initialize ();

private:

    Framebuffer* gBuffer;

    GLShaderProgram& GetShaderMeshObjectProgram (const MeshObject& meshObject);

};

#endif