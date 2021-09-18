#ifndef _GLFORWARDGEOMETRYRENDERSTAGE_H_
#define _GLFORWARDGEOMETRYRENDERSTAGE_H_

#include "cilantroengine.h"
#include "graphics/GLGeometryRenderStage.h"

class GLForwardGeometryRenderStage : public GLGeometryRenderStage
{
public:
    __EAPI GLForwardGeometryRenderStage ();
    __EAPI virtual ~GLForwardGeometryRenderStage ();

    __EAPI virtual void Initialize ();
    __EAPI virtual void Deinitialize ();

    virtual void OnFrame ();

protected:

    virtual void InitializeFramebuffer ();
    
private:

    virtual ShaderProgram& GetMeshObjectGeometryShaderProgram (const MeshObject& meshObject);
    virtual ShaderProgram& GetMeshObjectLightingShaderProgram (const MeshObject& meshObject);

};

#endif