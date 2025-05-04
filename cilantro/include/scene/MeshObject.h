#ifndef _MESHOBJECT_H_
#define _MESHOBJECT_H_

#include "cilantroengine.h"
#include "math/AABB.h"
#include "scene/GameObject.h"
#include "scene/BoneObject.h"
#include "scene/Material.h"
#include "resource/Mesh.h"
#include <vector>

struct IRenderer;

namespace cilantro {

// Represents a 3d mesh, inherits from GameObject
// 3d mesh has a collection of vertices and a collection of vertex indices to represent mesh faces (polygons)
// 3d mesh may have only one material assigned to it
class __CEAPI MeshObject : public GameObject
{
public:
    __EAPI MeshObject (std::shared_ptr<GameScene> gameScene, const std::string& meshName, const std::string& materialName);
    __EAPI virtual ~MeshObject ();

    // access mesh
    __EAPI std::shared_ptr<Mesh> GetMesh ();

    // material 
    __EAPI std::shared_ptr<MeshObject> SetMaterial (const std::string& materialName);
    __EAPI std::shared_ptr<Material> GetMaterial () const;

    // get axis aligned bounding box of mesh object
    __EAPI AABB GetAABB ();
    __EAPI std::shared_ptr<MeshObject> InvalidateAABB ();

    // add bone to mesh object
    __EAPI std::shared_ptr<MeshObject> AddInfluencingBoneObject (std::shared_ptr<BoneObject> boneObject);

    // generate array of bone transformations to be loaded to shader
    __EAPI float* GetBoneTransformationsMatrixArray ();

    // invoked by game loop on each frame or on update (e.g. transform)
    __EAPI virtual void OnFrame () override;
    __EAPI virtual void OnDraw (IRenderer& renderer) override;
    __EAPI virtual void OnUpdate (IRenderer& renderer) override;

private:
    std::shared_ptr<Mesh> m_mesh;
    std::vector<std::shared_ptr<BoneObject>> m_influencingBoneObjects;
    std::shared_ptr<Material> m_material;

    AABB m_aabb;
    bool m_aabbDirty;

    float m_boneTransformationMatrixArray[CILANTRO_MAX_BONES * 16];
};

} // namespace cilantro

#endif
