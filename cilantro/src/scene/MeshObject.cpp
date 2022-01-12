#include "cilantroengine.h"
#include "scene/GameScene.h"
#include "scene/MeshObject.h"
#include "scene/Material.h"
#include "math/Vector3f.h"
#include "math/Mathf.h"
#include "graphics/GeometryRenderStage.h"
#include "system/Game.h"
#include "system/CallbackProvider.h"
#include "system/LogMessage.h"

#include <vector>
#include <unordered_map>

MeshObject::MeshObject (const std::string& meshName, const std::string materialName) :
mesh (Game::GetResourceManager ().GetByName<Mesh> (meshName)),
material (Game::GetGameScene ().GetMaterialManager ().GetByName<Material> (materialName))
{
    mesh.RegisterCallback ("OnUpdateMesh", [&] (handle_t objectHandle) { InvokeCallbacks ("OnUpdateMeshObject", this->GetHandle (), 0); });
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
    material = Game::GetGameScene ().GetMaterialManager ().GetByName<Material> (materialName);

    return *this;
}

Material& MeshObject::GetMaterial () const
{
    return material;
}

void MeshObject::OnFrame ()
{
    GameObject::OnFrame ();
}

void MeshObject::OnDraw (GeometryRenderStage& renderStage)
{
    GameObject::OnDraw (renderStage);
    renderStage.Draw (*this);
}


void MeshObject::OnUpdate (GeometryRenderStage& renderStage)
{
    GameObject::OnUpdate (renderStage);
    renderStage.Update (*this);
}
