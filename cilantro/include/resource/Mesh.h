#ifndef _MESH_H_
#define _MESH_H_

#include "cilantroengine.h"
#include "resource/Resource.h"
#include "system/HookProvider.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include <cstdint>

namespace cilantro {

class Material;

struct Vector3Hash
{
    std::size_t operator() (const Vector3f& v) const noexcept
    {
        return std::hash<float>{}(v[0]) ^ std::hash<float>{}(v[1]) ^ std::hash<float>{}(v[2]);
    }
};

class Mesh : public Resource, public Hook<std::string>
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
    __EAPI size_t GetVertexCount () const;
    __EAPI size_t GetFaceCount () const;
    __EAPI size_t GetIndexCount () const;

    // get raw data
    __EAPI float* GetVerticesData ();
    __EAPI float* GetNormalsData ();
    __EAPI float* GetUVData ();
    __EAPI float* GetTangentData ();
    __EAPI float* GetBitangentData ();
    __EAPI uint32_t* GetBoneIndicesData ();
    __EAPI float* GetBoneWeightsData ();
    __EAPI std::vector<handle_t>& GetMeshBones ();

    // get faces raw data
    __EAPI uint32_t* GetFacesData ();

    // add vertices and primitives
    __EAPI Mesh& AddVertex (const Vector3f& vertex, const Vector2f& uv);
    __EAPI Mesh& AddFace (size_t v1, size_t v2, size_t v3);
    __EAPI Mesh& AddNormal (const Vector3f& normal);
    __EAPI Mesh& AddTangentBitangent (const Vector3f& tangent, const Vector3f& bitangent);
    __EAPI Mesh& AddVertexBoneInfluence (size_t v, float weight, handle_t boneHandle);


private:
    uint32_t GetFaceVertexIndex (size_t face, unsigned int faceVertex) const;

    Vector3f GetVertex (size_t index) const;
    Vector2f GetUV (size_t index) const;
    Vector3f GetNormal (size_t index) const;
    Vector3f GetTangent (size_t index) const;
    Vector3f GetBitangent (size_t index) const;

    Mesh& SetNormal (size_t index, const Vector3f& normal);
    Mesh& SetTangent (size_t index, const Vector3f& tangent);
    Mesh& SetBitangent (size_t index, const Vector3f& bitangent);

    bool smoothNormals;

    // index in this vector is bone index, element is bone handle in resource manager
    std::vector<handle_t> meshBones;

    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    std::vector<float> normals;
    std::vector<float> uvs;
    std::vector<float> tangents;
    std::vector<float> bitangents;

    std::vector<size_t> boneInfluenceCounts;
    std::vector<uint32_t> boneInfluenceIndices;
    std::vector<float> boneInfluenceWeights;

};

} // namespace cilantro

#endif