#ifndef _GEOMETRYRENDERSTAGE_H_
#define _GEOMETRYRENDERSTAGE_H_

#include "cilantroengine.h"
#include "graphics/RenderStage.h"

class MeshObject;
class PointLight;
class DirectionalLight;
class SpotLight;
class Material;

class GeometryRenderStage : public RenderStage
{
public:
    __EAPI GeometryRenderStage ();
    __EAPI virtual ~GeometryRenderStage ();

    // object drawing and updating
    virtual void Draw (MeshObject& meshObject) = 0;
    virtual void Update (MeshObject& meshObject) = 0;
    virtual void Update (PointLight& pointLight) = 0;
    virtual void Update (DirectionalLight& directionalLight) = 0;
    virtual void Update (SpotLight& spotLight) = 0;
    virtual void Update (Material& material, unsigned int textureUnit) = 0;
    virtual void Update (Material& material) = 0;
};



#endif
