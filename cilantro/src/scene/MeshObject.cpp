#include "cilantroengine.h"
#include "scene/GameScene.h"
#include "scene/BoneObject.h"
#include "scene/MeshObject.h"
#include "scene/Material.h"
#include "math/Vector3f.h"
#include "math/Mathf.h"
#include "system/Game.h"
#include "system/Hook.h"
#include "system/LogMessage.h"

#include <vector>
#include <unordered_map>
#include <cstring>
#include <span>

namespace cilantro
{

MeshObject::MeshObject (std::shared_ptr<GameScene> gameScene, const std::string& meshName, const std::string& materialName)
    : GameObject (gameScene)
{
    m_mesh = GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<Mesh> (meshName);
    m_material = GetGameScene ()->GetMaterialManager ()->GetByName<Material> (materialName);
    m_aabbDirty = true;

    // in case of mesh update, publish message to bus (recipients incl. for renderer)
    m_mesh->SubscribeHook ("OnUpdateMesh", [&] () 
    { 
        GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<MeshObjectUpdateMessage> (std::make_shared<MeshObjectUpdateMessage> (this->GetHandle ())); 
    });

    // in case of transform update, invalidate AABB
    GetModelTransform ()->SubscribeHook ("OnUpdateTransform", [&]() 
    {
        InvalidateAABB ();
    });
}

MeshObject::~MeshObject ()
{
}

std::shared_ptr<Mesh> MeshObject::GetMesh ()
{
    return m_mesh;
}

std::shared_ptr<MeshObject> MeshObject::SetMaterial (const std::string& materialName)
{
    m_material = m_gameScene.lock ()->GetMaterialManager ()->GetByName<Material> (materialName);

    return std::dynamic_pointer_cast<MeshObject> (shared_from_this ());
}

std::shared_ptr<Material> MeshObject::GetMaterial () const
{
    return m_material;
}

AABB MeshObject::GetAABB ()
{
    if (m_aabbDirty)
    {
        m_aabb = GetGameScene ()->GetRenderer ()->CalculateAABB (std::dynamic_pointer_cast<MeshObject> (shared_from_this ()));
        m_aabbDirty = false;

        InvalidateHierarchyAABB ();
    }
    
    return m_aabb;
}

std::shared_ptr<MeshObject> MeshObject::AddInfluencingBoneObject (std::shared_ptr<BoneObject> boneObject)
{
    auto it = std::find (m_influencingBoneObjects.begin (), m_influencingBoneObjects.end (), boneObject);

    if (it == m_influencingBoneObjects.end ())
    {
        // bone not found, add it
        m_influencingBoneObjects.push_back (boneObject);
    }

    boneObject->AddInfluencedMeshObject (std::dynamic_pointer_cast<MeshObject> (shared_from_this ()));

    // update renderer buffers
    GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<MeshObjectUpdateMessage> (std::make_shared<MeshObjectUpdateMessage> (this->GetHandle ())); 

    return std::dynamic_pointer_cast<MeshObject> (shared_from_this ());
}

float* MeshObject::GetBoneTransformationsMatrixArray (bool transpose)
{
    unsigned int index = 16;
    Matrix4f boneTransformation;
    Matrix4f identity;
    identity.InitIdentity ();

    // copy identity matrix in index 0
    std::memcpy (m_boneTransformationMatrixArray, identity[0], 16 * sizeof (float));

    // copy remaining bones
    for (handle_t boneHandle : m_mesh->GetMeshBones ())
    {
        std::shared_ptr<BoneObject> boneObject = nullptr;
        
        // find meshbone by bone handle
        for (auto b : m_influencingBoneObjects)
        {
            if (b->GetBone ()->GetHandle () == boneHandle)
            {
                boneObject = b;
                break;
            }
        }

        boneTransformation = transpose ? Mathf::Transpose(boneObject->GetWorldTransformMatrix () * boneObject->GetBone ()->GetOffsetMatrix ()) : (boneObject->GetWorldTransformMatrix () * boneObject->GetBone ()->GetOffsetMatrix ());

        std::memcpy (m_boneTransformationMatrixArray + index, boneTransformation[0], 16 * sizeof (float));
        index += 16;
    }

    return m_boneTransformationMatrixArray;
}

void MeshObject::OnFrame ()
{
    GameObject::OnFrame ();
}

void MeshObject::OnDraw (IRenderer& renderer)
{
    GameObject::OnDraw (renderer);
    renderer.Draw (std::dynamic_pointer_cast<MeshObject> (shared_from_this ()));
}

void MeshObject::OnUpdate (IRenderer& renderer)
{
    GameObject::OnUpdate (renderer);
    renderer.Update (std::dynamic_pointer_cast<MeshObject> (shared_from_this ()));
}

} // namespace cilantro