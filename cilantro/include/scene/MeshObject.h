#ifndef _MESHOBJECT_H_
#define _MESHOBJECT_H_

#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "scene/Material.h"
#include "math/Vector3f.h"
#include "math/Curve.h"
#include <vector>

class Renderer;

// Represents a 3d mesh, inherits from GameObject
// 3d mesh has a collection of vertices and a collection of vertex indices to represent mesh faces (polygons)
// 3d mesh may have only one material assigned to it
class MeshObject : public GameObject
{
public:
    __EAPI MeshObject ();
    __EAPI virtual ~MeshObject ();

    // Cleans up contents of used collections
    __EAPI MeshObject& Clear ();

    // Initialize mesh in local space as unit primitives with center in space origin
    __EAPI MeshObject& InitUnitCube ();
    __EAPI MeshObject& InitUnitSphere (unsigned int subdivisions, bool sharedVertices = false);

    // calculate vertex normals
    __EAPI void CalculateVertexNormals ();

    // get mesh counts
    __EAPI unsigned int GetVertexCount ();
    __EAPI unsigned int GetIndexCount ();

    // set mesh material
    __EAPI MeshObject& SetMaterial (Material& material);

    // get mesh material
    __EAPI Material& GetMaterial () const;

    // get vertices raw data
    __EAPI float* GetVerticesData ();

    // get normals raw data
    __EAPI float* GetNormalsData ();

    // get faces raw data
    __EAPI unsigned int* GetFacesData ();

    // add vertices and primitives
    __EAPI void AddVertex (const Vector3f& vertex);
    __EAPI void AddFace (unsigned int v1, unsigned int v2, unsigned int v3);

    // invoked by game loop on each frame or on update (e.g. transform)
    virtual void OnFrame ();
    virtual void OnDraw (Renderer& renderer);
    virtual void OnUpdate (Renderer& renderer);

private:

    Material* objectMaterial;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    std::vector<float> normals;
    std::vector<float> uvs;
};

#endif
