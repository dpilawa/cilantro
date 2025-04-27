#include "scene/BoneObject.h"
#include "scene/GameScene.h"

namespace cilantro {

BoneObject::BoneObject (std::shared_ptr<GameScene> gameScene, std::shared_ptr<Bone> bone) : GameObject (gameScene)
{
    m_bone = bone;
}

BoneObject::~BoneObject ()
{
}

std::shared_ptr<Bone> BoneObject::GetBone () const
{
    return m_bone;
}

std::shared_ptr<BoneObject> BoneObject::AddInfluencedMeshObject (std::shared_ptr<MeshObject> meshObject)
{
    m_influencedMeshObjects.insert (meshObject);

    return std::dynamic_pointer_cast<BoneObject> (shared_from_this ());
}

} // namespace cilantro