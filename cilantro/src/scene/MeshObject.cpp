#include "cilantroengine.h"
#include "scene/GameScene.h"
#include "scene/MeshObject.h"
#include "scene/Material.h"
#include "math/Vector3f.h"
#include "math/Mathf.h"
#include "graphics/Renderer.h"
#include "system/EngineContext.h"
#include "system/CallbackProvider.h"
#include "system/LogMessage.h"

#include <vector>
#include <unordered_map>

MeshObject::MeshObject (const std::string& meshName, const std::string materialName) :
mesh (EngineContext::GetGameScene ().GetMaterialManager ().GetByName<Mesh> (meshName)),
material (EngineContext::GetGameScene ().GetMaterialManager ().GetByName<Material> (materialName))
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
    material = EngineContext::GetGameScene ().GetMaterialManager ().GetByName<Material> (materialName);

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

void MeshObject::OnDraw (Renderer& renderer)
{
    GameObject::OnDraw (renderer);
    renderer.Draw (*this);
}


void MeshObject::OnUpdate (Renderer& renderer)
{
    GameObject::OnUpdate (renderer);
    renderer.Update (*this);
}
