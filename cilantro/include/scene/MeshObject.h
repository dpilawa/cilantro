#ifndef _MESHOBJECT_H_
#define _MESHOBJECT_H_

#include "cilantroengine.h"

#include "scene/GameObject.h"
#include "scene/Material.h"
#include "resource/Mesh.h"
#include <vector>

class Renderer;

// Represents a 3d mesh, inherits from GameObject
// 3d mesh has a collection of vertices and a collection of vertex indices to represent mesh faces (polygons)
// 3d mesh may have only one material assigned to it
class MeshObject : public GameObject
{
public:
    MeshObject () = delete;
    __EAPI MeshObject (const std::string& meshName, const std::string materialName);
    __EAPI virtual ~MeshObject ();

    // access mesh
    __EAPI Mesh& GetMesh ();

    // set mesh material
    __EAPI MeshObject& SetMaterial (const std::string& materialName);
    // get mesh material
    __EAPI Material& GetMaterial () const;

    // invoked by game loop on each frame or on update (e.g. transform)
    virtual void OnFrame ();
    virtual void OnDraw (Renderer& renderer);
    virtual void OnUpdate (Renderer& renderer);

private:
    Mesh& mesh;
    Material& material;
};

#endif
