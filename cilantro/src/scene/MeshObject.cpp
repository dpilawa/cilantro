#include "cilantroengine.h"
#include "scene/GameScene.h"
#include "scene/Bone.h"
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

namespace cilantro
{

MeshObject::MeshObject (std::shared_ptr<GameScene> gameScene, const std::string& meshName, const std::string& materialName)
    : GameObject (gameScene)
{
    m_mesh = GetGameScene ()->GetGame ()->GetResourceManager ()->GetByName<Mesh> (meshName);
    m_material = GetGameScene ()->GetMaterialManager ()->GetByName<Material> (materialName);

    // in case of mesh update, publish message to bus (recipients incl. for renderer)
    m_mesh->SubscribeHook ("OnUpdateMesh", [&] () 
    { 
        GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<MeshObjectUpdateMessage> (std::make_shared<MeshObjectUpdateMessage> (this->GetHandle ())); 
    });

    // in case of transform update, set aab dirty flag so that it is recalculated when queried
    GetModelTransform ()->SubscribeHook ("OnUpdateTransform", [&]() 
    {
        m_aabbDirty = true; 
        GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<MeshObjectUpdateMessage> (std::make_shared<MeshObjectUpdateMessage> (this->GetHandle ())); 
    });

    m_aabbDirty = true;
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
        m_aabb.CalculateForMesh (m_mesh, GetWorldTransformMatrix ());
        m_aabbDirty = false;
    }

    return m_aabb;
}

float* MeshObject::GetBoneTransformationsMatrixArray ()
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
        auto b = m_gameScene.lock ()->GetGameObjectManager ()->GetByHandle<Bone> (boneHandle);
        boneTransformation = b->GetWorldTransformMatrix () * b->GetOffsetMatrix ();

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