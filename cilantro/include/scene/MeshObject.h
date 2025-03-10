#ifndef _MESHOBJECT_H_
#define _MESHOBJECT_H_

#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "scene/Material.h"
#include "resource/Mesh.h"
#include <vector>

struct IRenderer;

namespace cilantro {

// Represents a 3d mesh, inherits from GameObject
// 3d mesh has a collection of vertices and a collection of vertex indices to represent mesh faces (polygons)
// 3d mesh may have only one material assigned to it
class MeshObject : public GameObject
{
public:
    __EAPI MeshObject (CGameScene* gameScene, const std::string& meshName, const std::string& materialName);
    __EAPI virtual ~MeshObject ();

    // access mesh
    __EAPI Mesh& GetMesh ();

    // material 
    __EAPI MeshObject& SetMaterial (const std::string& materialName);
    __EAPI Material& GetMaterial () const;

    // generate array of bone transformations to be loaded to shader
    float* GetBoneTransformationsMatrixArray ();

    // invoked by game loop on each frame or on update (e.g. transform)
    virtual void OnFrame () override;
    virtual void OnDraw (IRenderer& renderer) override;
    virtual void OnUpdate (IRenderer& renderer) override;

private:
    Mesh& mesh;
    Material& material;

    float boneTransformationMatrixArray[CILANTRO_MAX_BONES * 16];
};

} // namespace cilantro

#endif
