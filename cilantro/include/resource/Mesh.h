#ifndef _MESH_H_
#define _MESH_H_

#include "cilantroengine.h"
#include "resource/Resource.h"
#include "system/CallbackProvider.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"

class Material;

struct Vector3Hash
{
    std::size_t operator() (const Vector3f& v) const noexcept
    {
        return std::hash<float>{}(v[0]) ^ std::hash<float>{}(v[1]) ^ std::hash<float>{}(v[2]);
    }
};

class Mesh : public Resource, public CallbackProvider<std::string, handle_t>
{

public:

    __EAPI Mesh ();
    __EAPI virtual ~Mesh ();

    // Cleans up contents of used collections
    __EAPI Mesh& Clear ();

    // calculate vertex normals
    __EAPI Mesh& CalculateVertexNormals ();
    __EAPI Mesh& SetSmoothNormals (bool smoothNormals);

    // calculate tangents and bitangents
    __EAPI Mesh& CalculateTangentsBitangents ();

    // get mesh counts
    __EAPI unsigned int GetVertexCount () const;
    __EAPI unsigned int GetFaceCount () const;
    __EAPI unsigned int GetIndexCount () const;

    // get raw data
    __EAPI float* GetVerticesData ();
    __EAPI float* GetNormalsData ();
    __EAPI float* GetUVData ();
    __EAPI float* GetTangentData ();
    __EAPI float* GetBitangentData ();

    // get faces raw data
    __EAPI unsigned int* GetFacesData ();

    // add vertices and primitives
    __EAPI Mesh& AddVertex (const Vector3f& vertex, const Vector2f& uv);
    __EAPI Mesh& AddFace (unsigned int v1, unsigned int v2, unsigned int v3);

private:
    unsigned int GetFaceVertexIndex (unsigned int face, unsigned int faceVertex) const;

    Vector3f GetVertex (unsigned int index) const;
    Vector2f GetUV (unsigned int index) const;
    Vector3f GetNormal (unsigned int index) const;
    Vector3f GetTangent (unsigned int index) const;
    Vector3f GetBitangent (unsigned int index) const;

    Mesh& SetNormal (unsigned int index, const Vector3f& normal);
    Mesh& SetTangent (unsigned int index, const Vector3f& tangent);
    Mesh& SetBitangent (unsigned int index, const Vector3f& bitangent);

    std::string materialName;
    bool smoothNormals;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<float> normals;
    std::vector<float> uvs;
    std::vector<float> tangents;
    std::vector<float> bitangents;

};

#endif