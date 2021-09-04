#ifndef _GLDEFERREDRENDERER_H_
#define _GLDEFERREDRENDERER_H_

#include "cilantroengine.h"
#include "graphics/GLRenderer.h"
#include <set>

class GLRenderStage;

class GLDeferredRenderer : public GLRenderer
{
public:
    __EAPI GLDeferredRenderer (unsigned int width, unsigned int height);
    __EAPI virtual ~GLDeferredRenderer ();

    __EAPI virtual void Initialize ();

    virtual void RenderFrame ();
    virtual void Update (Material& material);

private:

    // set of handles of distinct lighting pass shader programs used in the scene
    std::set <handle_t> lightingShaders;

    unsigned int lightingShaderStagesCount;

    virtual ShaderProgram& GetMeshObjectGeometryShaderProgram (const MeshObject& meshObject);
    virtual ShaderProgram& GetMeshObjectLightingShaderProgram (const MeshObject& meshObject);

};

#endif