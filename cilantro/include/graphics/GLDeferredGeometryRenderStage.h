#ifndef _GLDEFERREDGEOMETRYRENDERSTAGE_H_
#define _GLDEFERREDGEOMETRYRENDERSTAGE_H_

#include "cilantroengine.h"
#include "graphics/GLGeometryRenderStage.h"
#include <set>

class GLRenderStage;

class GLDeferredGeometryRenderStage : public GLGeometryRenderStage
{
public:
    __EAPI GLDeferredGeometryRenderStage ();
    __EAPI virtual ~GLDeferredGeometryRenderStage ();

    __EAPI virtual void Initialize ();
    __EAPI virtual void Deinitialize ();

    virtual void OnFrame ();
    virtual void Update (Material& material);

protected:

    virtual void InitializeFramebuffer ();

private:

    // set of handles of distinct lighting pass shader programs used in the scene
    std::set <handle_t> lightingShaders;

    unsigned int lightingShaderStagesCount;

    virtual std::string GetMeshObjectGeometryShaderProgram (const MeshObject& meshObject);
    virtual std::string GetMeshObjectLightingShaderProgram (const MeshObject& meshObject);

};

#endif