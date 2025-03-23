#include "cilantroengine.h"
#include "scene/GameScene.h"
#include "scene/Bone.h"
#include "scene/MeshObject.h"
#include "scene/Material.h"
#include "math/Vector3f.h"
#include "math/Mathf.h"
#include "system/Game.h"
#include "system/HookProvider.h"
#include "system/LogMessage.h"

#include <vector>
#include <unordered_map>
#include <cstring>

namespace cilantro
{

MeshObject::MeshObject (std::shared_ptr<GameScene> gameScene, const std::string& meshName, const std::string& materialName)
    : GameObject (gameScene)
{
    m_mesh = GetGameScene ()->GetGame ()->GetResourceManager ().GetByName<Mesh> (meshName);
    m_material = GetGameScene ()->GetMaterialManager ().GetByName<Material> (materialName);
    m_mesh->SubscribeHook ("OnUpdateMesh", [&] () { GetGameScene ()->GetGame ()->GetMessageBus ()->Publish<MeshObjectUpdateMessage> (std::make_shared<MeshObjectUpdateMessage> (this->GetHandle ())); });
}

MeshObject::~MeshObject ()
{
}

std::shared_ptr<Mesh> MeshObject::GetMesh ()
{
    return m_mesh;
}

MeshObject& MeshObject::SetMaterial (const std::string& materialName)
{
    m_material = m_gameScene.lock ()->GetMaterialManager ().GetByName<Material> (materialName);

    return *this;
}

std::shared_ptr<Material> MeshObject::GetMaterial () const
{
    return m_material;
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
        auto b = m_gameScene.lock ()->GetGameObjectManager ().GetByHandle<Bone> (boneHandle);
        boneTransformation = b->GetModelTransformMatrix () * b->GetOffsetMatrix ();

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
    renderer.Draw (std::dynamic_pointer_cast<MeshObject> (this->GetPointer ()));
}

void MeshObject::OnUpdate (IRenderer& renderer)
{
    GameObject::OnUpdate (renderer);
    renderer.Update (std::dynamic_pointer_cast<MeshObject> (this->GetPointer ()));
}

} // namespace cilantro