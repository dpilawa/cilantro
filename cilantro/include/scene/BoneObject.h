#ifndef _BONEOBJECT_H_
#define _BONEOBJECT_H_

#include "cilantroengine.h"
#include "resource/Bone.h"
#include "scene/GameObject.h"
#include "math/Matrix4f.h"
#include <unordered_set>

namespace cilantro {

class MeshObject;

class __CEAPI BoneObject : public GameObject
{
public:
    __EAPI BoneObject (std::shared_ptr<GameScene> gameScene, std::shared_ptr<Bone> bone);
    __EAPI virtual ~BoneObject ();

    __EAPI std::shared_ptr<Bone> GetBone () const;
    __EAPI std::shared_ptr<BoneObject> AddInfluencedMeshObject (std::shared_ptr<MeshObject> meshObject);

private:
    std::shared_ptr<Bone> m_bone;
    std::unordered_set<std::shared_ptr<MeshObject>> m_influencedMeshObjects;

};

} // namespace cilantro

#endif