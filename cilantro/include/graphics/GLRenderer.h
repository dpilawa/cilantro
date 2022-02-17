#ifndef _GLRENDERER_H_
#define _GLRENDERER_H_

#include "cilantroengine.h"
#include "graphics/Renderer.h"

class GameScene;
class GeometryBuffer;

class GLRenderer : public Renderer
{
public:
    __EAPI GLRenderer (GameScene* gameScene, unsigned int width, unsigned int height);
    __EAPI virtual ~GLRenderer ();

    __EAPI virtual void Initialize ();
    __EAPI virtual void Deinitialize ();
    __EAPI virtual void RenderFrame ();
    __EAPI virtual void RenderGeometryBuffer (GeometryBuffer* buffer);    

    // framebuffer control
    virtual Framebuffer* CreateFramebuffer (unsigned int rgbTextures, unsigned int rgbaTextures, bool multisampleEnabled);
    virtual void BindDefaultFramebuffer ();

    virtual void ClearColorBuffer (Vector4f& rgba);
    virtual void ClearDepthBuffer ();

    virtual void SetDepthTestEnabled (bool value);
    virtual void SetFaceCullingEnabled (bool value);
    virtual void SetMultisamplingEnabled (bool value);

    virtual void SetStencilTestEnabled (bool value);
    virtual void SetStencilTestFunction (StencilTestFunction testFunction, int testValue);

private:

    void InitializeShaderLibrary ();

    void InitializeQuadGeometryBuffer ();
    void DeinitializeQuadGeometryBuffer ();

};

#endif