#ifndef _MESHOBJECT_H_
#define _MESHOBJECT_H_

#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "scene/Material.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include "math/Curve.h"
#include <vector>

class Renderer;

struct Vector3Hash
{
    std::size_t operator() (const Vector3f& v) const noexcept
    {
        return std::hash<float>{}(v[0]) ^ std::hash<float>{}(v[1]) ^ std::hash<float>{}(v[2]);
    }
};

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

    // calculate vertex normals
    __EAPI MeshObject& CalculateVertexNormals ();
    __EAPI MeshObject& SetSmoothNormals (bool smoothNormals);

    // get mesh counts
    __EAPI unsigned int GetVertexCount () const;
    __EAPI unsigned int GetFaceCount () const;
    __EAPI unsigned int GetIndexCount () const;

    // set mesh material
    __EAPI MeshObject& SetMaterial (const std::string& materialName);

    // get mesh material
    __EAPI Material& GetMaterial () const;

    // get raw data
    __EAPI float* GetVerticesData ();
    __EAPI float* GetNormalsData ();
    __EAPI float* GetUVData ();

    // get faces raw data
    __EAPI unsigned int* GetFacesData ();

    // add vertices and primitives
    __EAPI void AddVertex (const Vector3f& vertex, const Vector2f& uv);
    __EAPI void AddFace (unsigned int v1, unsigned int v2, unsigned int v3);

    // invoked by game loop on each frame or on update (e.g. transform)
    virtual void OnFrame ();
    virtual void OnDraw (Renderer& renderer);
    virtual void OnUpdate (Renderer& renderer);

private:

    Vector3f GetVertex (unsigned int vertex) const;
    unsigned int GetFaceVertexIndex (unsigned int face, unsigned int faceVertex) const;
    Vector3f GetNormal (unsigned int vertex) const;
    MeshObject& SetNormal (unsigned int vertex, const Vector3f& normal);

    Material* objectMaterial;

    bool smoothNormals;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    std::vector<float> normals;
    std::vector<float> uvs;
};

#endif
