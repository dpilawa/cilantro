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

MeshObject::MeshObject (GameScene* gameScene, const std::string& meshName, const std::string& materialName)
    : GameObject (gameScene)
    , mesh (Game::GetResourceManager ().GetByName<Mesh> (meshName))
    , material (gameScene->GetMaterialManager ().GetByName<Material> (materialName))
{
    mesh.SubscribeHook ("OnUpdateMesh", [&] () { Game::GetMessageBus ().Publish<MeshObjectUpdateMessage> (std::make_shared<MeshObjectUpdateMessage> (this->GetHandle ())); });
}

MeshObject::~MeshObject ()
{
}

Mesh& MeshObject::GetMesh ()
{
    return mesh;
}

MeshObject& MeshObject::SetMaterial (const std::string& materialName)
{
    material = gameScene->GetMaterialManager ().GetByName<Material> (materialName);

    return *this;
}

Material& MeshObject::GetMaterial () const
{
    return material;
}

float* MeshObject::GetBoneTransformationsMatrixArray ()
{
    unsigned int index = 16;
    Matrix4f boneTransformation;
    Matrix4f identity;
    identity.InitIdentity ();

    // copy identity matrix in index 0
    std::memcpy (boneTransformationMatrixArray, identity[0], 16 * sizeof (float));

    // copy remaining bones
    for (handle_t boneHandle : mesh.GetMeshBones ())
    {
        Bone& b = gameScene->GetGameObjectManager ().GetByHandle<Bone> (boneHandle);
        boneTransformation = b.GetModelTransformMatrix () * b.GetOffsetMatrix ();

        std::memcpy (boneTransformationMatrixArray + index, boneTransformation[0], 16 * sizeof (float));
        index += 16;
    }

    return boneTransformationMatrixArray;
}

void MeshObject::OnFrame ()
{
    GameObject::OnFrame ();
}

void MeshObject::OnDraw (IRenderer& renderer)
{
    GameObject::OnDraw (renderer);
    renderer.Draw (*this);
}

void MeshObject::OnUpdate (IRenderer& renderer)
{
    GameObject::OnUpdate (renderer);
    renderer.Update (*this);
}

} // namespace cilantro