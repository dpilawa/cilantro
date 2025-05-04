#include "scene/BoneObject.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"

namespace cilantro {

BoneObject::BoneObject (std::shared_ptr<GameScene> gameScene, std::shared_ptr<Bone> bone) : GameObject (gameScene)
{
    m_bone = bone;
    m_influenceChanged = false;

    // in case of transform update, invalidate AABB of all influenced mesh objects
    GetModelTransform ()->SubscribeHook ("OnUpdateTransform", [&]() 
    { 
        for (auto&& meshObject : m_influencedMeshObjects)
        {
            meshObject->InvalidateAABB ();
            m_influenceChanged = true;
        }
    });

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

void BoneObject::OnFrame ()
{
    GameObject::OnFrame ();

    if (m_influenceChanged == true)
    {
        for (auto&& meshObject : m_influencedMeshObjects)
        {
            GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<TransformUpdateMessage> (std::make_shared<TransformUpdateMessage> (meshObject->GetHandle ()));
        }
    }

    m_influenceChanged = false;
}

} // namespace cilantro