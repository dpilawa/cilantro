#include "cilantroengine.h"
#include "resource/Mesh.h"
#include "scene/Material.h"
#include "math/Vector3f.h"
#include "math/Mathf.h"
#include "system/CallbackProvider.h"
#include "system/LogMessage.h"

#include <vector>
#include <unordered_map>

Mesh::Mesh () : Resource ()
{
    this->smoothNormals = false;
}

Mesh::~Mesh ()
{
}

Mesh& Mesh::Clear ()
{
    vertices.clear ();
    normals.clear ();
    indices.clear ();

    InvokeCallbacks ("OnUpdateMesh", this->GetHandle ());

    return *this;
}

Mesh& Mesh::CalculateVertexNormals ()
{
    Vector3f normal;
    Vector3f va;
    Vector3f v0, v1, v2;
    std::unordered_map<Vector3f, std::vector<unsigned int>, Vector3Hash> facesForVertex;

    // zero normals
    normals.clear ();
    for (unsigned int v = 0; v < GetVertexCount () * 3; v++)
    {
        normals.push_back (0.0f);
    }

    if (!smoothNormals)
    {
        // loop through all faces
        for (unsigned int f = 0; f < GetFaceCount (); f++)
        {
            // get face vertices
            v0 = GetVertex (GetFaceVertexIndex (f, 0));
            v1 = GetVertex (GetFaceVertexIndex (f, 1));
            v2 = GetVertex (GetFaceVertexIndex (f, 2));

            // calculate normal
            normal = Mathf::Cross (v1 - v0, v2 - v0);

            // add normal to all vertices of face
            SetNormal (GetFaceVertexIndex (f, 0), GetNormal (GetFaceVertexIndex (f, 0)) + normal);
            SetNormal (GetFaceVertexIndex (f, 1), GetNormal (GetFaceVertexIndex (f, 1)) + normal);
            SetNormal (GetFaceVertexIndex (f, 2), GetNormal (GetFaceVertexIndex (f, 2)) + normal);
        }

    }
    else
    {
        // loop through all faces
        for (unsigned int f = 0; f < GetFaceCount (); f++)
        {
            // get face vertices
            v0 = GetVertex (GetFaceVertexIndex (f, 0));
            v1 = GetVertex (GetFaceVertexIndex (f, 1));
            v2 = GetVertex (GetFaceVertexIndex (f, 2));

            facesForVertex[v0].push_back (GetFaceVertexIndex (f, 0));
            facesForVertex[v1].push_back (GetFaceVertexIndex (f, 1));
            facesForVertex[v2].push_back (GetFaceVertexIndex (f, 2));
        }

        // loop through all faces
        for (unsigned int f = 0; f < GetFaceCount (); f++)
        {
            // get face vertices
            v0 = GetVertex (GetFaceVertexIndex (f, 0));
            v1 = GetVertex (GetFaceVertexIndex (f, 1));
            v2 = GetVertex (GetFaceVertexIndex (f, 2));

            // calculate normal
            normal = Mathf::Cross (v1 - v0, v2 - v0);

            // add normal to all vertices of adjacent faces
            for (auto&& n : facesForVertex[v0])
            {
                SetNormal (n, GetNormal (n) + normal);
            }

            for (auto&& n : facesForVertex[v1])
            {
                SetNormal (n, GetNormal (n) + normal);
            }

            for (auto&& n : facesForVertex[v2])
            {
                SetNormal (n, GetNormal (n) + normal);
            }

        }

    }

    // normalize normals
    for (unsigned int v = 0; v < GetVertexCount (); v++)
    {
        normal = GetNormal (v);
        SetNormal (v, Mathf::Normalize (normal));
    }

    return *this;
}

Mesh& Mesh::CalculateTangentsBitangents ()
{
    Vector3f v0, v1, v2;
    Vector2f uv0, uv1, uv2;
    Vector3f edge1, edge2;
    Vector2f deltaUV1, deltaUV2;
    Vector3f tangent;
    Vector3f bitangent;
    float detInv;

    tangents.clear ();
    bitangents.clear ();
    tangents.resize (GetVertexCount () * 3);
    bitangents.resize (GetVertexCount () * 3);

    // loop through all faces
    for (unsigned int f = 0; f < GetFaceCount (); f++)
    {
        // get face vertices
        v0 = GetVertex (GetFaceVertexIndex (f, 0));
        v1 = GetVertex (GetFaceVertexIndex (f, 1));
        v2 = GetVertex (GetFaceVertexIndex (f, 2));

        // get face texture coordinates
        uv0 = GetUV (GetFaceVertexIndex (f, 0));
        uv1 = GetUV (GetFaceVertexIndex (f, 1));
        uv2 = GetUV (GetFaceVertexIndex (f, 2));

        // face edge deltas
        edge1 = v1 - v0;
        edge2 = v2 - v0;
        deltaUV1 = uv1 - uv0;
        deltaUV2 = uv2 - uv2; 

        // calculate tangents and bitangents
        detInv = 1.0f / (deltaUV1[0] * deltaUV2[1] - deltaUV2[0] * deltaUV1[1]);

        tangent = detInv * (deltaUV2[1] * edge1 - deltaUV1[1] * edge2);
        bitangent = detInv * (deltaUV1[0] * edge2 - deltaUV2[0] * edge1);

        // set tangent and bitangent on all vertices of a face
        SetTangent (GetFaceVertexIndex (f, 0), tangent);
        SetTangent (GetFaceVertexIndex (f, 1), tangent);
        SetTangent (GetFaceVertexIndex (f, 2), tangent);
        SetBitangent (GetFaceVertexIndex (f, 0), bitangent);
        SetBitangent (GetFaceVertexIndex (f, 1), bitangent);
        SetBitangent (GetFaceVertexIndex (f, 2), bitangent);
    }

    return *this;
}

Mesh& Mesh::SetSmoothNormals (bool smoothNormals)
{
    this->smoothNormals = smoothNormals;
    this->CalculateVertexNormals ();
    InvokeCallbacks ("OnUpdateMesh", this->GetHandle ());

    return *this;
}

unsigned int Mesh::GetVertexCount () const
{
    return (unsigned int) (vertices.size () / 3);
}

unsigned int Mesh::GetFaceCount () const
{
    return (unsigned int) (indices.size () / 3);
}

unsigned int Mesh::GetIndexCount () const
{
    return (unsigned int) indices.size ();
}

float* Mesh::GetVerticesData ()
{
    return vertices.data ();
}

float* Mesh::GetNormalsData ()
{
    return normals.data ();
}

float* Mesh::GetUVData ()
{
    return uvs.data ();
}

float* Mesh::GetTangentData ()
{
    return tangents.data ();
}

float* Mesh::GetBitangentData ()
{
    return bitangents.data ();
}

unsigned int* Mesh::GetFacesData ()
{
    return indices.data ();
}

Mesh& Mesh::AddVertex (const Vector3f& vertex, const Vector2f& uv)
{
    vertices.push_back (vertex[0]);
    vertices.push_back (vertex[1]);
    vertices.push_back (vertex[2]);

    uvs.push_back (uv[0]);
    uvs.push_back (uv[1]);

    return *this;
}

Mesh& Mesh::AddFace (unsigned int v1, unsigned int v2, unsigned int v3)
{
    indices.push_back (v1);
    indices.push_back (v2);
    indices.push_back (v3);

    return *this;
}

unsigned int Mesh::GetFaceVertexIndex (unsigned int face, unsigned int faceVertex) const
{
    return indices[face * 3 + faceVertex];
}

Vector3f Mesh::GetVertex (unsigned int index) const
{
    return Vector3f (vertices[index * 3], vertices[index * 3 + 1], vertices[index * 3 + 2]);
}

Vector2f Mesh::GetUV (unsigned int index) const
{
    return Vector2f (uvs[index * 2], uvs[index * 2 + 1]);
}

Vector3f Mesh::GetNormal (unsigned int index) const
{
    return Vector3f (normals[index * 3], normals[index * 3 + 1], normals[index * 3 + 2]);
}

Vector3f Mesh::GetTangent (unsigned int index) const
{
    return Vector3f (tangents[index * 3], tangents[index * 3 + 1], tangents[index * 3 + 2]);
}

Vector3f Mesh::GetBitangent (unsigned int index) const
{
    return Vector3f (bitangents[index * 3], bitangents[index * 3 + 1], bitangents[index * 3 + 2]);
}

Mesh& Mesh::SetNormal (unsigned int index, const Vector3f& normal)
{
    normals[index * 3] = normal[0];
    normals[index * 3 + 1] = normal[1];
    normals[index * 3 + 2] = normal[2];

    return *this;
}

Mesh& Mesh::SetTangent (unsigned int index, const Vector3f& tangent)
{
    tangents[index * 3] = tangent[0];
    tangents[index * 3 + 1] = tangent[1];
    tangents[index * 3 + 2] = tangent[2];

    return *this;
}

Mesh& Mesh::SetBitangent (unsigned int index, const Vector3f& bitangent)
{
    bitangents[index * 3] = bitangent[0];
    bitangents[index * 3 + 1] = bitangent[1];
    bitangents[index * 3 + 2] = bitangent[2];

    return *this;
}