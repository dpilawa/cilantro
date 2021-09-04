#ifndef _GLFORWARDRENDERER_H_
#define _GLFORWARDRENDERER_H_

#include "cilantroengine.h"
#include "graphics/GLRenderer.h"

class GLForwardRenderer : public GLRenderer
{
public:
    __EAPI GLForwardRenderer (unsigned int width, unsigned int height);
    __EAPI virtual ~GLForwardRenderer ();

    __EAPI virtual void Initialize ();

    virtual void RenderFrame ();

private:

    virtual ShaderProgram& GetMeshObjectGeometryShaderProgram (const MeshObject& meshObject);
    virtual ShaderProgram& GetMeshObjectLightingShaderProgram (const MeshObject& meshObject);

};

#endif